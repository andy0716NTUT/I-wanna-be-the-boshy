"""Run the opt-in real-GL tuning checks silently; keep stage images for review."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess
from PIL import Image

ROOT = Path(__file__).absolute().parents[1]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--config", choices=["Debug", "Release"], default="Debug")
    parser.add_argument("--size", choices=["native", "1080p", "4k"], default="native")
    args = parser.parse_args()
    output = ROOT / "build/gameplay-tuning" / (args.config + ("" if args.size == "native" else "-"+args.size))
    output.mkdir(parents=True, exist_ok=True)
    env = os.environ.copy()
    env.update(SDL_AUDIODRIVER="dummy", PATH="C:/Windows/System32;C:/Windows")
    if args.size != "native":
        width, height = (1920, 1080) if args.size == "1080p" else (3840, 2160)
        env.update(BOSHY_VISUAL_WIDTH=str(width), BOSHY_VISUAL_HEIGHT=str(height))
    startup = subprocess.STARTUPINFO()
    startup.dwFlags |= subprocess.STARTF_USESHOWWINDOW
    startup.wShowWindow = 0
    exe = ROOT / "build/windows/bin" / args.config / "GameplayTuningProbe.exe"
    with (output / "probe.log").open("wb") as log:
        subprocess.run([str(exe), str(output)], cwd="C:/Windows", env=env,
                       startupinfo=startup, stdout=log, stderr=subprocess.STDOUT,
                       check=True, timeout=90)
    report = json.loads((output / "result.json").read_text())
    report.update(config=args.config, display=args.size, base_revision="b28f807b1f3e71e7ce7be40dd649ff3de8392009",
                  binary_sha256=hashlib.sha256(exe.read_bytes()).hexdigest(), images={})
    for ppm in sorted(output.glob("*.ppm")):
        image = Image.open(ppm).convert("RGB")
        png = ppm.with_suffix(".png")
        image.save(png)
        report["images"][png.name] = hashlib.sha256(png.read_bytes()).hexdigest()
    region = (800, 704, 896, 768)
    for stage, source in [("idle", "phase9.png"), ("gone", "phasenone.png")]:
        frame = Image.open(output / f"stone-{stage}.png").convert("RGB")
        scale = min(frame.width//1280, frame.height//960)
        if scale < 1:
            raise AssertionError("Expected native or larger drawable")
        left, top = (frame.width-1280*scale)//2, (frame.height-960*scale)//2
        actual = frame.crop((left, top, left+1280*scale, top+960*scale)).resize((1280,960), Image.Resampling.NEAREST).crop(region)
        expected = Image.open(ROOT / "Resources/Image/Background/WORLD1" / source).convert("RGB").crop(region)
        if actual.tobytes() != expected.tobytes():
            raise AssertionError(f"Stone {stage} pixels differ from original {source} region")
    report["original_idle_pixels"] = "PASS"
    report["original_background_after_fall"] = "PASS"
    (output / "report.json").write_text(json.dumps(report, indent=2)+"\n", encoding="utf-8")
    print(json.dumps(report, indent=2), flush=True)


if __name__ == "__main__":
    main()
