"""Independently check scenery candidate file hashes, size, alpha and protected pixels."""
import argparse
import hashlib
import json
from pathlib import Path

import numpy as np
from PIL import Image

ROOT = Path(__file__).absolute().parents[1]


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--manifest", type=Path, required=True)
    args = p.parse_args()
    manifest = json.loads(args.manifest.read_text(encoding="utf-8"))
    inventory = json.loads((ROOT/"art/remaster-v1/source-inventory.json").read_text(encoding="utf-8"))
    originals = {e["path"]:e for e in inventory["files"]}
    actual_sources = {p.relative_to(ROOT).as_posix() for p in (ROOT/"Resources/Image").rglob("*.png")}
    if actual_sources != set(originals):
        raise AssertionError("Original PNG set differs from checkpoint")
    for path, entry in originals.items():
        if digest(ROOT/path) != entry["sha256"]:
            raise AssertionError(f"Original changed: {path}")
    selected = {path for path in originals if "/Background/" in path or "/MapObject/" in path}
    entries = manifest["entries"]
    if len(entries) != len(selected) or {e["source"] for e in entries} != selected:
        raise AssertionError("Missing, duplicate or unexpected scenery mapping")
    base = (ROOT/manifest["candidate_directory"]).resolve()
    base.relative_to(ROOT.resolve())
    changed = preserved = protected_count = painted_count = 0
    for entry in entries:
        path = (ROOT/entry["candidate"])
        path.resolve().relative_to(base)
        if digest(path) != entry["candidate_sha256"]:
            raise AssertionError(f"Candidate checksum differs: {path}")
        with Image.open(ROOT/entry["source"]) as old, Image.open(path) as new:
            if new.format != "PNG" or new.size != old.size:
                raise AssertionError(f"Canvas/format differs: {path}")
            a, b = np.array(old.convert("RGBA")), np.array(new.convert("RGBA"))
            if not np.array_equal(a[:,:,3], b[:,:,3]):
                raise AssertionError(f"Original alpha/anchor bounds differ: {path}")
            if hashlib.sha256(b[:,:,3].tobytes()).hexdigest() != originals[entry["source"]]["alpha_sha256"]:
                raise AssertionError(f"Original alpha checksum differs: {path}")
        if entry["status"].startswith("PRESERVED_"):
            if digest(path) != originals[entry["source"]]["sha256"]:
                raise AssertionError(f"Preserved critical/data image changed: {path}")
            preserved += 1
        else:
            if digest(path) == originals[entry["source"]]["sha256"]:
                raise AssertionError(f"Candidate labelled changed but identical: {path}")
            changed += 1
        if "mask" in entry:
            mask_path = ROOT/entry["mask"]
            mask_path.resolve().relative_to(base)
            if digest(mask_path) != entry["mask_sha256"]:
                raise AssertionError(f"Review mask checksum differs: {path}")
            with Image.open(mask_path) as image:
                mask = np.array(image.convert("L"))
            if mask.shape != a.shape[:2] or not np.isin(mask,[0,255]).all():
                raise AssertionError(f"Malformed protection mask: {path}")
            keep = mask == 255
            if int(keep.sum()) != entry["protected_pixels"] or not np.array_equal(a[keep], b[keep]):
                raise AssertionError(f"Protected original pixels changed: {path}")
            protected_count += int(keep.sum())
            paint_path = ROOT/entry["texture_mask"]
            paint_path.resolve().relative_to(base)
            if digest(paint_path) != entry["texture_mask_sha256"]:
                raise AssertionError(f"Material mask checksum differs: {path}")
            with Image.open(paint_path) as image:
                paint = np.array(image.convert("L"))
            if paint.shape != keep.shape or not np.isin(paint,[0,255]).all():
                raise AssertionError(f"Malformed material mask: {path}")
            if (keep & (paint==255)).any() or int((paint==255).sum()) != entry["texture_pixels"]:
                raise AssertionError(f"Protected/material region overlap: {path}")
            painted_count += int((paint==255).sum())
            if entry.get("background_status") == "ORIGINAL_HELD_REGISTRATION_SEAMS":
                if not np.array_equal(a[paint==0],b[paint==0]):
                    raise AssertionError(f"Held original backdrop changed: {path}")
            # An independent, map-derived visible spike landmark check.
            source = Path(entry["source"])
            world, phase = source.parent.name, source.stem[5:]
            phase = "4_1" if world=="WORLD1" and phase=="4" else phase
            map_path = ROOT/"Resources/Image/Background"/world/f"Map{phase}.txt"
            grid = np.array([[int(v) for v in row.replace(","," ").split()]
                             for row in map_path.read_text(encoding="utf-8").splitlines() if row.strip()])
            spike_tiles = (grid==5).repeat(16,0).repeat(16,1)
            rgb = a[:,:,:3]
            landmarks = spike_tiles & (rgb.min(2)>150) & (rgb.max(2)-rgb.min(2)<30)
            if not np.array_equal(a[landmarks],b[landmarks]):
                raise AssertionError(f"Visible spike landmarks changed: {path}")
    for item in manifest["materials"]:
        if digest(ROOT/item["source"]) != item["source_sha256"]:
            raise AssertionError("Original terrain reference source changed")
        if digest(ROOT/item["reference"]) != item["reference_sha256"]:
            raise AssertionError("Terrain reference crop changed")
        with Image.open(ROOT/item["source"]) as old, Image.open(ROOT/item["reference"]) as ref, Image.open(ROOT/item["candidate"]) as new:
            original = old.convert("RGBA").crop(item["box"])
            if original.tobytes() != ref.convert("RGBA").tobytes():
                raise AssertionError("Terrain reference does not match original crop")
            if new.size != original.size or new.convert("RGBA").getchannel("A").tobytes() != original.getchannel("A").tobytes():
                raise AssertionError("Material tile size/alpha differs")
        if digest(ROOT/item["candidate"]) != item["candidate_sha256"]:
            raise AssertionError("Material tile checksum differs")
    for group in ("input_assets", "prompt_files"):
        for path, expected in manifest[group].items():
            if digest(ROOT/path) != expected:
                raise AssertionError(f"Generation provenance changed: {path}")
    candidate_paths = {e["source"]:ROOT/e["candidate"] for e in entries}
    with Image.open(candidate_paths["Resources/Image/Background/WORLD1/phase9.png"]) as room, \
         Image.open(candidate_paths["Resources/Image/Background/WORLD1/phasenone.png"]) as backdrop, \
         Image.open(candidate_paths["Resources/Image/MapObject/platform.png"]) as stone:
        region = (800,704,896,768)  # Existing TippingStone::Home and original96x64 footprint.
        composite = backdrop.convert("RGBA").crop(region)
        composite.alpha_composite(stone.convert("RGBA"))
        if room.convert("RGBA").crop(region).tobytes() != composite.tobytes():
            raise AssertionError("Room9 stone sprite and background cover are inconsistent")
    if manifest.get("runtime_deployed") is not False:
        raise AssertionError("This validator certifies candidates, not runtime deployment")
    print(json.dumps({"result":"PASS", "original_pngs":len(originals),
                      "selected_scenery_pngs":len(selected), "changed_candidates":changed,
                      "preserved_critical_and_data_files":preserved,
                      "protected_pixels_checked":protected_count,
                      "material_pixels_mapped":painted_count,
                      "material_tiles":len(manifest["materials"]),
                      "canvas_alpha_and_spike_landmarks":"PASS",
                      "originals_and_generation_provenance":"PASS",
                      "room9_stone_cover_pixel_coherence":"PASS",
                      "runtime_deployed":False,
                      "limit":"Approximate foreground masks require visual review; no gameplay or texture-equivalence certification"},indent=2))


if __name__ == "__main__":
    main()
