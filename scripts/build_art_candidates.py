"""Assemble a separate scenery candidate set; never write to Resources or deploy."""
import argparse
from collections import deque
from io import BytesIO
import hashlib
import html
import json
import os
from pathlib import Path
import warnings

import numpy as np
from PIL import Image, ImageFilter, ImageDraw, ImageFont

ROOT = Path(__file__).absolute().parents[1]
ART = ROOT / "art/remaster-v1"


def sha(data):
    return hashlib.sha256(data).hexdigest()


def rgba(path):
    with Image.open(path) as image:
        return image.convert("RGBA")


def write_once(path, data):
    path.parent.mkdir(parents=True, exist_ok=True)
    if path.exists():
        if path.read_bytes() != data:
            raise FileExistsError(f"Preserve existing variant; use a new --output: {path}")
    else:
        with path.open("xb") as stream:
            stream.write(data)


def png(path, image):
    stream = BytesIO()
    image.save(stream, format="PNG")
    data = stream.getvalue()
    write_once(path, data)
    return sha(data)


def map_grid(world, phase):
    if world == "WORLD1" and phase == "4":
        phase = "4_1"  # Existing identical visual alias, not a map-format change.
    path = ROOT / "Resources/Image/Background" / world / f"Map{phase}.txt"
    return np.array([[int(v) for v in line.replace(",", " ").split()]
                     for line in path.read_text(encoding="utf-8").splitlines() if line.strip()])


def pixel_tiles(world, phase):
    grid = map_grid(world, phase)
    if grid.shape != (60, 80):
        raise ValueError(f"Unexpected map dimensions: {world}/{phase}")
    return grid.repeat(16, 0).repeat(16, 1)


def shared_background(world, phases):
    """Median only at map-declared empty pixels; unknown areas stay protected.

    This is an approximate visual-layer reconstruction, not collision data.
    Row chunks bound temporary memory; no missing background is hallucinated.
    """
    images = np.stack([np.array(rgba(ROOT / "Resources/Image/Background" / world /
                                   f"phase{phase}.png"))[:, :, :3] for phase in phases])
    empty = np.stack([(lambda tiles: (tiles == 0) | (tiles >= 66))(pixel_tiles(world, phase))
                      for phase in phases])
    known = empty.any(0)
    result = images[0].copy()
    for y in range(0, 960, 64):
        values = images[:, y:y+64].astype(np.float32)
        values[~empty[:, y:y+64]] = np.nan
        with warnings.catch_warnings():
            warnings.simplefilter("ignore", RuntimeWarning)
            median = np.nanmedian(values, axis=0)
        good = known[y:y+64]
        result[y:y+64][good] = np.rint(median[good]).astype(np.uint8)
    return result, known


def fit_object(raw, original):
    """Mechanical format/geometry fitting after the actual image_gen redraw.

    Crop at >=128 alpha to discard model-added transparent padding/glow, fit
    into the original alpha bounds with nearest sampling, extend edge colors
    into any uncovered pixels, then restore the exact original alpha channel.
    No silhouette, canvas, anchor or collision size is authored by this step.
    """
    alpha = original.getchannel("A")
    box = alpha.getbbox()
    crop = raw.getchannel("A").point(lambda value: 255 if value >= 128 else 0).getbbox()
    if not box or not crop:
        raise ValueError("Empty source/model alpha")
    resized = np.array(raw.crop(crop).resize((box[2]-box[0], box[3]-box[1]), Image.Resampling.NEAREST))
    rgb = resized[:, :, :3].copy()
    good = resized[:, :, 3] >= 128
    # Multi-source nearest edge-color fill at these tiny final object dimensions.
    todo = deque(zip(*np.nonzero(good)))
    if not todo:
        raise ValueError("No opaque generated object pixels")
    while todo:
        y, x = todo.popleft()
        for yy, xx in ((y-1,x),(y+1,x),(y,x-1),(y,x+1)):
            if 0 <= yy < good.shape[0] and 0 <= xx < good.shape[1] and not good[yy,xx]:
                rgb[yy,xx] = rgb[y,x]
                good[yy,xx] = True
                todo.append((yy,xx))
    result = Image.new("RGBA", original.size)
    result.paste(Image.fromarray(rgb).convert("RGBA"), box[:2])
    result.putalpha(alpha)
    return result, {"raw_crop": list(crop), "original_bounds": list(box)}


def repaint_terrain(pixels, final, tiles, spike_art, protected, templates, green):
    """Swap only positively matched source patches or solid green interiors.

    Template matching uses the original unedited image and six opaque anchors,
    followed by a full opaque-pixel check (max error8). Never move a patch or
    paint over white spike landmarks. All output alpha remains the source alpha.
    """
    painted = np.zeros(pixels.shape[:2], bool)
    counts = {}
    for name, old, new, solid_only in templates:
        h, w = old.shape[:2]
        opaque = old[:,:,3] >= 250
        if not opaque.any():
            continue
        indices = np.argwhere(opaque)
        anchors = indices[np.linspace(0,len(indices)-1,6,dtype=int)]
        yy, xx = np.meshgrid(np.arange(0,pixels.shape[0]-h+1,16),
                             np.arange(0,pixels.shape[1]-w+1,16), indexing="ij")
        good = np.ones(xx.shape, bool)
        for ay, ax in anchors:
            delta = np.abs(pixels[yy+ay,xx+ax,:3].astype(np.int16)-old[ay,ax,:3].astype(np.int16))
            good &= delta.max(2)<=8
        count = 0
        for y, x in zip(yy[good], xx[good]):
            region = np.s_[y:y+h,x:x+w]
            if painted[region][old[:,:,3]>0].any() or spike_art[region][old[:,:,3]>0].any():
                continue
            if solid_only and not np.isin(tiles[region],[1,2]).all():
                continue
            delta = np.abs(pixels[region][:,:,:3].astype(np.int16)-old[:,:,:3].astype(np.int16))
            if (delta[opaque]>8).any():
                continue
            alpha = old[:,:,3:4].astype(np.uint16)
            target = final[region]
            target[:,:,:3] = ((new[:,:,:3].astype(np.uint16)*alpha +
                               target[:,:,:3].astype(np.uint16)*(255-alpha)+127)//255).astype(np.uint8)
            painted[region] |= old[:,:,3]>0
            count += 1
        counts[name] = count
    if green is not None:
        # Preserve the existing black outline and highlights; only recolor the
        # saturated green interior of map-declared solid platform rectangles.
        rgb = pixels[:,:,:3]
        interior = np.isin(tiles,[1,2]) & protected & ~painted
        interior &= (rgb[:,:,1]>130) & (rgb[:,:,0]<100) & (rgb[:,:,2]<90)
        interior &= rgb[:,:,1].astype(np.int16)>rgb[:,:,0].astype(np.int16)*2
        h, w = green.shape[:2]
        y, x = np.indices(interior.shape)
        final[interior,:3] = green[y[interior]%h,x[interior]%w,:3]
        painted |= interior
        counts["green_interior_pixels"] = int(interior.sum())
    return final, painted, counts


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", type=Path, default=ART / "candidates")
    args = parser.parse_args()
    output = args.output.absolute()
    relative = output.resolve().relative_to(ROOT.resolve())
    if not relative.parts or relative.parts[0] not in ("art", "build"):
        raise ValueError("Candidates must stay under workspace art/ or build/; never Resources")
    inventory = json.loads((ART / "source-inventory.json").read_text(encoding="utf-8"))
    for entry in inventory["files"]:
        if sha((ROOT / entry["path"]).read_bytes()) != entry["sha256"]:
            raise ValueError(f"Original changed since checkpoint: {entry['path']}")
    mountain = rgba(ART / "reference/world1-mountain-approved.png").resize((1280,960), Image.Resampling.NEAREST)
    pink = rgba(ART / "raw/backdrops/world1-heart.png").resize((1280,960), Image.Resampling.NEAREST)
    hills = rgba(ART / "raw/backdrops/world2-hills.png").resize((1280,960), Image.Resampling.NEAREST)
    original_mountain = np.array(rgba(ROOT / "Resources/Image/Background/WORLD1/phasenone.png"))[:, :, :3]
    pink_ref, pink_known = shared_background("WORLD1", ["13","14"])
    hills_ref, hills_known = shared_background("world2", ["1","2_1","2_2","3","4","5","6"])
    catalog = json.loads((ART/"reference/terrain/catalog.json").read_text(encoding="utf-8"))
    materials = []
    material_arrays = {}
    for item in catalog:
        reference = rgba(ROOT/item["reference"])
        assert sha((ROOT/item["source"]).read_bytes()) == item["source_sha256"]
        assert rgba(ROOT/item["source"]).crop(item["box"]).tobytes() == reference.tobytes()
        raw_path = ART/"raw/terrain"/(item["id"]+".png")
        raw = rgba(raw_path)
        crop = (0,0,*raw.size)
        if item["id"] == "world1-grass-top":
            # The model added black bars. Trim them only in this derived copy;
            # retain the untouched raw image and record its exact crop.
            visible = np.array(raw)[:,:,:3].max(2)>12
            crop = Image.fromarray(visible.astype(np.uint8)*255).getbbox()
        normalized = raw.crop(crop).resize(reference.size,Image.Resampling.NEAREST)
        normalized.putalpha(reference.getchannel("A"))
        path = output/"materials"/(item["id"]+".png")
        value = dict(item, candidate=path.relative_to(ROOT).as_posix(),
                     candidate_sha256=png(path,normalized), raw_crop=list(crop),
                     raw_input=raw_path.relative_to(ROOT).as_posix())
        materials.append(value)
        material_arrays[item["id"]] = (np.array(reference),np.array(normalized))
    object_templates = []
    for name in ("platform.png","small_ground.png"):
        old = rgba(ROOT/"Resources/Image/MapObject"/name)
        new, _ = fit_object(rgba(ART/"raw/objects"/name),old)
        object_templates.append((name,np.array(old),np.array(new),False))
    entries = []
    for item in inventory["files"]:
        source = Path(item["path"])
        if "/Background/" not in source.as_posix() and "/MapObject/" not in source.as_posix():
            continue  # Scope: scenery; characters/animation/UI are a separate batch.
        original = rgba(ROOT / source)
        pixels = np.array(original)
        record = {"source": source.as_posix(), "source_sha256": item["sha256"],
                  "source_size": list(original.size), "source_alpha_sha256": item["alpha_sha256"]}
        preserved = False
        if "for collision" in source.stem:
            result = original
            record.update(status="PRESERVED_DATA_REFERENCE", reason="Collision helper visualization is not redrawn")
            preserved = True
        elif source.name in ("phase2trap1.png", "phase2trap2.png"):
            result = original
            record.update(status="PRESERVED_CRITICAL_HAZARD", reason="Exact moving-spike formation retained; no verified redraw of its geometry")
            preserved = True
        elif "/MapObject/" in source.as_posix():
            result, fitting = fit_object(rgba(ART / "raw/objects" / source.name), original)
            record.update(status="CANDIDATE_ALPHA_ALIGNED", fitting=fitting,
                          raw_input=(ART / "raw/objects" / source.name).relative_to(ROOT).as_posix(),
                          reason="AI-redrawn texture, original canvas and alpha restored exactly; visual review required")
        elif source.name == "phasenone.png":
            result = mountain.copy()
            result.putalpha(original.getchannel("A"))
            record.update(status="CANDIDATE_BACKGROUND", protected_pixels=0,
                          raw_input=(ART / "reference/world1-mountain-approved.png").relative_to(ROOT).as_posix())
        else:
            world, phase = source.parent.name, source.stem[5:]
            if world == "WORLD1" and phase not in ("13","14"):
                reference, known, backdrop = original_mountain, np.ones((960,1280),bool), mountain
                raw_input = ART / "reference/world1-mountain-approved.png"
            elif world == "WORLD1":
                reference, known, backdrop = pink_ref, pink_known, pink
                raw_input = ART / "raw/backdrops/world1-heart.png"
            else:
                reference, known, backdrop = hills_ref, hills_known, hills
                raw_input = ART / "raw/backdrops/world2-hills.png"
            protected = (np.abs(pixels[:,:,:3].astype(np.int16)-reference.astype(np.int16)).max(2)>8) | ~known
            # Preserve visible neutral-white/gray spike pixels even if a pale
            # backdrop makes the background-difference classifier ambiguous.
            tiles = pixel_tiles(world, phase)
            original_backdrop = world=="WORLD1" and phase=="7"
            if original_backdrop:
                # Reject the failed registration, rather than shipping seams.
                protected[:] = True
            elif world == "world2" or phase in ("13","14"):
                # These maps bake slightly different old backgrounds. Confine
                # inferred foreground to map-object neighborhoods, rather than
                # accidentally keeping old clouds/heart fragments in empty sky.
                occupied = ((tiles>0) & (tiles<66)).astype(np.uint8)*255
                nearby = np.array(Image.fromarray(occupied).filter(ImageFilter.MaxFilter(17)))>0
                protected &= nearby
                if world == "world2":
                    # Decorative orange pillars are not always collision tiles.
                    orange = (pixels[:,:,0]>200) & (pixels[:,:,1]>80) & (pixels[:,:,2]<130)
                    protected |= orange
                # Within spike cells, colored old scenery in the triangle's
                # empty corners is background, not a part of the white/gray
                # spike. Retain red heart details if a source overlaps a cell.
                red = (pixels[:,:,0]>200) & (pixels[:,:,1]<180) & (pixels[:,:,2]<180)
                protected &= (tiles!=5) | neutral | red
            neutral = pixels[:,:,:3].max(2)-pixels[:,:,:3].min(2) < 30
            spike_art = (tiles==5) & neutral & (pixels[:,:,:3].min(2)>150)
            protected |= spike_art
            final = pixels.copy() if original_backdrop else np.array(backdrop).copy()
            final[protected] = pixels[protected]
            final[:,:,3] = pixels[:,:,3]
            templates = list(object_templates)
            if world == "WORLD1":
                for name in ("world1-wall","world1-grass-top"):
                    old, new = material_arrays[name]
                    templates.append((name,old,new,True))
            green = material_arrays["world2-green-fill"][1] if world=="world2" else None
            final, painted, texture_counts = repaint_terrain(pixels,final,tiles,spike_art,protected,templates,green)
            protected &= ~painted
            result = Image.fromarray(final)
            assert np.array_equal(final[protected], pixels[protected])
            assert np.array_equal(final[spike_art], pixels[spike_art])
            mask_path = output / "masks" / source.relative_to("Resources")
            mask_sha = png(mask_path, Image.fromarray(protected.astype(np.uint8)*255))
            paint_path = output/"texture-masks"/source.relative_to("Resources")
            paint_sha = png(paint_path,Image.fromarray(painted.astype(np.uint8)*255))
            record.update(status="CANDIDATE_COMPOSITE_REVIEW_REQUIRED",
                          protected_pixels=int(protected.sum()), spike_pixels_preserved=int(spike_art.sum()),
                          unknown_background_pixels=int((~known).sum()), mask=mask_path.relative_to(ROOT).as_posix(),
                          mask_sha256=mask_sha, raw_input=raw_input.relative_to(ROOT).as_posix(),
                          texture_mask=paint_path.relative_to(ROOT).as_posix(), texture_mask_sha256=paint_sha,
                          texture_pixels=int(painted.sum()), texture_matches=texture_counts,
                          background_status="ORIGINAL_HELD_REGISTRATION_SEAMS" if original_backdrop else "REDRAW_CANDIDATE",
                          reason="Backdrop/material candidates with protected non-painted foreground; masks are approximate, not a runtime parity certificate")
            if original_backdrop:
                record["status"] = "CANDIDATE_TERRAIN_ONLY_ORIGINAL_BACKGROUND"
        if not preserved and result.tobytes() == original.tobytes():
            preserved = True
            record["status"] = "PRESERVED_NO_COMPATIBLE_CHANGE"
        assert result.size == original.size
        assert result.getchannel("A").tobytes() == original.getchannel("A").tobytes()
        path = output / source.relative_to("Resources")
        if preserved:
            write_once(path, (ROOT / source).read_bytes())
            digest = sha(path.read_bytes())
            assert digest == item["sha256"]
        else:
            digest = png(path, result)
        record.update(candidate=path.relative_to(ROOT).as_posix(), candidate_sha256=digest,
                      alpha_sha256=sha(result.getchannel("A").tobytes()), runtime_deployed=False)
        entries.append(record)
    for item in inventory["files"]:
        assert sha((ROOT / item["path"]).read_bytes()) == item["sha256"]
    raw_inputs = sorted(set(e["raw_input"] for e in entries+materials if "raw_input" in e))
    prompts = sorted((ART / "prompts").glob("*.txt"))
    manifest = {"schema":1, "scope":"Backgrounds and scene objects; character/animation/UI PNGs remain original",
                "rollback_tag":inventory["rollback_tag"], "original_png_count":inventory["png_count"],
                "all_originals_unchanged":True, "runtime_deployed":False,
                "candidate_directory":output.relative_to(ROOT).as_posix(),
                "input_assets":{path:sha((ROOT/path).read_bytes()) for path in raw_inputs},
                "prompt_files":{p.relative_to(ROOT).as_posix():sha(p.read_bytes()) for p in prompts},
                "review_status":"NEEDS_VISUAL_REVIEW", "entries":entries}
    manifest["materials"] = materials
    write_once(output / "manifest.json", (json.dumps(manifest,indent=2)+"\n").encode("utf-8"))
    cards = []
    review = output / "review"
    review.mkdir(parents=True, exist_ok=True)
    for e in entries:
        old = os.path.relpath(ROOT/e["source"], review).replace("\\", "/")
        new = os.path.relpath(ROOT/e["candidate"], review).replace("\\", "/")
        cards.append(f'<section><h2>{html.escape(e["source"])}</h2><p>{e["status"]}</p>'
                     f'<div><figure><a href="{html.escape(old)}"><img src="{html.escape(old)}"></a><figcaption>Original {e["source_size"]}</figcaption></figure>'
                     f'<figure><a href="{html.escape(new)}"><img src="{html.escape(new)}"></a><figcaption>Candidate {e["source_size"]}</figcaption></figure></div></section>')
    for e in materials:
        old = os.path.relpath(ROOT/e["reference"],review).replace("\\","/")
        new = os.path.relpath(ROOT/e["candidate"],review).replace("\\","/")
        cards.append(f'<section><h2>Material: {html.escape(e["id"])}</h2><div>'
                     f'<figure><a href="{html.escape(old)}"><img src="{html.escape(old)}"></a><figcaption>Original crop {e["size"]}</figcaption></figure>'
                     f'<figure><a href="{html.escape(new)}"><img src="{html.escape(new)}"></a><figcaption>Candidate {e["size"]}</figcaption></figure></div></section>')
    page = ('<!doctype html><meta charset="utf-8"><title>Scenery candidates v1</title>'
            '<style>body{background:#18222b;color:#e6edf4;font:16px sans-serif;margin:24px}'
            'section{margin:32px 0}h2{font-size:18px}section div{display:flex;gap:16px}'
            'figure{margin:0;width:48%}img{width:100%;max-width:100%;image-rendering:pixelated;background:#32434e}'
            'figcaption{padding:8px}</style><h1>Scenery candidates v1</h1>'
            '<p>Separate preview only. Original resources and default game remain unchanged. '
            'Composite masks are approximate; preserve gameplay geometry during review.</p>'+''.join(cards))
    write_once(review / "index.html", page.encode("utf-8"))
    # Offline comparison artifact, not a game screenshot or replacement asset.
    overview = Image.new("RGB",(1120,1344),"#1b2832")
    draw = ImageDraw.Draw(overview)
    font_path = Path("C:/Windows/Fonts/segoeui.ttf")
    font = ImageFont.truetype(str(font_path),17) if font_path.exists() else ImageFont.load_default()
    samples = ["Resources/Image/Background/WORLD1/phase9.png",
               "Resources/Image/Background/WORLD1/phase13.png",
               "Resources/Image/Background/world2/phase4.png"]
    for row, name in enumerate(samples):
        e = next(e for e in entries if e["source"]==name)
        for col, key in enumerate(("source","candidate")):
            image = rgba(ROOT/e[key]).resize((560,420),Image.Resampling.NEAREST)
            overview.paste(image,(col*560,row*448+28))
            draw.text((col*560+8,row*448+5),f'{Path(name).parent.name}/{Path(name).name} - {"Original" if col==0 else "Candidate"}',font=font,fill="white")
    png(review/"overview.png",overview)
    changed = sum(e["candidate_sha256"] != e["source_sha256"] for e in entries)
    print(json.dumps({"result":"PASS", "selected":len(entries), "changed_candidates":changed,
                      "intentionally_preserved":len(entries)-changed,
                      "all119_original_pngs_unchanged":True,"sizes_and_alpha":"PASS",
                      "gallery":str(review/"index.html"),"manifest":str(output/"manifest.json")},indent=2))


if __name__ == "__main__":
    main()
