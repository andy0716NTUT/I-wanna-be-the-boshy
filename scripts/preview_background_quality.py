"""Make one non-AI mountain-background candidate and review sheets; never deploy it."""
import argparse
import hashlib
import json
from pathlib import Path

from PIL import Image, ImageChops, ImageEnhance, ImageFilter, ImageFont, ImageDraw

ROOT = Path(__file__).absolute().parents[1]
SOURCE = ROOT / "Resources/Image/Background/WORLD1/phasenone.png"
ROOM = ROOT / "Resources/Image/Background/WORLD1/phase9.png"


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def panel_pair(before, after, title, labels, font):
    sheet = Image.new("RGB", (1280, 780), "#151b23")
    draw = ImageDraw.Draw(sheet)
    draw.text((20, 10), title, font=font, fill="#eef4fc")
    for i, (image, label) in enumerate(zip((before, after), labels)):
        x = i * 640
        draw.text((x+20, 48), label, font=font, fill="#cedbea")
        # Overview only: both panels use the same downsample, independent of
        # the candidate's unchanged 1280x960 native size.
        sheet.paste(image.resize((640, 480), Image.Resampling.LANCZOS), (x, 88))
        draw.text((x+20, 578), "100% detail / no resize", font=font, fill="#cedbea")
        sheet.paste(image.crop((320, 220, 960, 380)), (x, 616))
    return sheet


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", type=Path,
                        default=ROOT / "build/asset-preview/world1-mountain-v1")
    args = parser.parse_args()
    output = args.output.absolute()
    output.mkdir(parents=True, exist_ok=False)  # Preserve any previous review.
    hashes = {p: digest(p) for p in (SOURCE, ROOM)}
    original = Image.open(SOURCE).convert("RGBA")
    rgb = original.convert("RGB")
    # Conservative photometric treatment only: no resampling, synthesized
    # texture, shape edits, deconvolution or invented high-resolution detail.
    corrected = ImageEnhance.Contrast(rgb).enhance(1.055)
    corrected = ImageEnhance.Color(corrected).enhance(1.025)
    corrected = corrected.filter(ImageFilter.UnsharpMask(radius=16, percent=12, threshold=4))
    corrected = corrected.filter(ImageFilter.UnsharpMask(radius=1.8, percent=65, threshold=3))
    candidate = corrected.convert("RGBA")
    candidate.putalpha(original.getchannel("A"))
    candidate.save(output / "phasenone-quality-v1.png")

    # Static in-room mockup: baked maps have up to 5 levels of rounding noise
    # in the common sky. A small difference threshold avoids a fragmented mask.
    # All pixels outside this approximate background mask remain byte-identical.
    # This is a comparison artifact, not a deployed map or in-game screenshot.
    room = Image.open(ROOM).convert("RGBA")
    if room.size != original.size:
        raise ValueError("Room/background geometry differs")
    pixels, source_pixels, new_pixels = list(room.getdata()), list(original.getdata()), list(candidate.getdata())
    matched = [max(abs(x-y) for x, y in zip(a[:3], b[:3])) <= 8 and a[3] == b[3]
               for a, b in zip(pixels, source_pixels)]
    room_preview = room.copy()
    room_preview.putdata([new if use else old for old, new, use in zip(pixels, new_pixels, matched)])
    room_preview.save(output / "phase9-background-mockup-v1.png")
    font = ImageFont.truetype("C:/Windows/Fonts/msjh.ttc", 22)
    panel_pair(rgb, corrected, "山景背景・傳統影像處理試作", ("原圖", "候選 v1：輕度對比／色彩／銳利化"), font).save(output / "comparison.png")
    panel_pair(room.convert("RGB"), room_preview.convert("RGB"),
               "第 9 關・背景套用示意（靜態合成，尚未套入遊戲）",
               ("原素材", "背景候選 v1；近似背景遮罩合成"), font).save(output / "room-comparison.png")

    reopened = Image.open(output / "phasenone-quality-v1.png").convert("RGBA")
    assert reopened.size == original.size == (1280, 960)
    assert ImageChops.difference(reopened.getchannel("A"), original.getchannel("A")).getbbox() is None
    assert all(a == b for a, b, use in zip(pixels, room_preview.getdata(), matched) if not use)
    assert all(digest(p) == before for p, before in hashes.items())
    report = {"result": "PASS", "method": "Pillow deterministic non-AI processing",
              "source": SOURCE.relative_to(ROOT).as_posix(), "source_sha256": hashes[SOURCE],
              "room_source_sha256": hashes[ROOM], "size": list(original.size),
              "settings": {"contrast": 1.055, "color": 1.025,
                           "local_contrast_unsharp": {"radius": 16, "percent": 12, "threshold": 4},
                           "edge_unsharp": {"radius": 1.8, "percent": 65, "threshold": 3}},
              "originals_unchanged": True, "dimensions_and_alpha_unchanged": True,
              "protected_room_pixels_unchanged": True, "mockup_mask_max_channel_difference": 8,
              "runtime_deployed": False,
              "limitation": "Original blur is retained; no lost detail has been recovered. Static room mockup uses approximate background classification, not a runtime foreground-extraction implementation.",
              "files": {p.name: digest(p) for p in sorted(output.glob("*.png"))}}
    (output / "report.json").write_text(json.dumps(report, ensure_ascii=False, indent=2)+"\n", encoding="utf-8")
    print(json.dumps(report, ensure_ascii=True, indent=2))


if __name__ == "__main__":
    main()
