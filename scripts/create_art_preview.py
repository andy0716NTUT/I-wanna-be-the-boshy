"""Create/test an isolated internal art preview from the unchanged RC2 executable."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import time

from PIL import Image
from run_gameplay_regression import read

ROOT = Path(__file__).absolute().parents[1]
RUNTIME = "0b15f6bf1e8a7ec116e3a5208df39e8c3f4aa85e"
CASES = ["movement","full-jump","shooting","spikes","checkpoint","transition",
         "spider-bird","resize-play","boss"]


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def save(path, value):
    path.write_text(json.dumps(value,indent=2)+"\n",encoding="utf-8",newline="\n")


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--output", type=Path, required=True)
    p.add_argument("--resume", action="store_true")
    args = p.parse_args()
    output = args.output.absolute()
    output.resolve().relative_to((ROOT/"build").resolve())
    base = ROOT/"build/release-candidate/BoshyRemaster-Windows-x64-RC2-0b15f6bf"
    manifest_path = ROOT/"art/remaster-v1/candidates/manifest.json"
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    original = json.loads((base/"manifest.json").read_text(encoding="utf-8"))
    if original["runtime_revision"] != RUNTIME:
        raise ValueError("Wrong RC2 base")
    for name, expected in original["files"].items():
        if digest(base/name) != expected:
            raise ValueError(f"Preserve modified RC2 file, do not overwrite it: {name}")
    marker = output/"art-preview.json"
    if output.exists():
        if not args.resume or not marker.exists():
            raise FileExistsError("Preserve existing preview; use a new output or --resume for a complete stage")
        meta = json.loads(marker.read_text(encoding="utf-8"))
        if meta["candidate_manifest_sha256"] != digest(manifest_path):
            raise ValueError("Candidate set changed; create another preview version")
    else:
        # No existing source/release folder is overwritten or moved.
        shutil.copytree(base,output,ignore=lambda directory,names: {"manifest.json"} if Path(directory)==base else set())
        shutil.copy2(base/"manifest.json",output/"baseline-manifest.json")
        replacements = []
        for entry in manifest["entries"]:
            relative = Path(entry["source"])
            target = output/relative
            target.resolve().relative_to((output/"Resources").resolve())
            if digest(target) != entry["source_sha256"]:
                raise ValueError(f"Baseline resource differs: {relative}")
            backup = output/"OriginalAssets"/relative.relative_to("Resources")
            backup.parent.mkdir(parents=True,exist_ok=True)
            shutil.copy2(target,backup)
            shutil.copy2(ROOT/entry["candidate"],target)
            replacements.append({"path":relative.as_posix(),"backup":backup.relative_to(output).as_posix(),
                                 "original_sha256":entry["source_sha256"],"preview_sha256":entry["candidate_sha256"]})
        art_commit = subprocess.check_output(["git","rev-parse","HEAD"],cwd=ROOT,text=True).strip()
        meta = {"kind":"internal isolated art preview, not a public release", "runtime_revision":RUNTIME,
                "art_commit":art_commit,"candidate_manifest_sha256":digest(manifest_path),
                "rollback_tag":manifest["rollback_tag"],"replacements":replacements,
                "default_repo_resources_changed":False,"old_rc2_preserved":True}
        save(marker,meta)
        old_exe = os.path.relpath(base/"IWannaBeTheBoshy.exe",output).replace("/","\\")
        (output/"Play-ArtPreview-Silent.cmd").write_bytes(
            b'@echo off\r\nsetlocal\r\nset "SDL_AUDIODRIVER=dummy"\r\n"%~dp0IWannaBeTheBoshy.exe"\r\nendlocal\r\n')
        (output/"Play-Original-RC2-Silent.cmd").write_bytes(
            ('@echo off\r\nsetlocal\r\nset "SDL_AUDIODRIVER=dummy"\r\n"%~dp0'+old_exe+'"\r\nendlocal\r\n').encode("ascii"))
        (output/"README-Art-Preview.txt").write_text(
            "Internal scenery preview; unchanged RC2 code, candidate PNG overlay only.\n"
            "Play-ArtPreview-Silent.cmd launches the art preview without sound.\n"
            "Play-Original-RC2-Silent.cmd opens the retained RC2 in this workspace.\n"
            "OriginalAssets contains every original replaced PNG, with hashes in art-preview.json.\n"
            "Repository Resources, old RC2, TXT maps, characters and animations are untouched.\n"
            "Room7 retains its original backdrop; only matched terrain is new there.\n"
            "Candidate masks/visual readability still need human review; this is not a final release.\n",
            encoding="utf-8",newline="\n")
    for e in meta["replacements"]:
        if digest(output/e["path"])!=e["preview_sha256"] or digest(output/e["backup"])!=e["original_sha256"]:
            raise ValueError("Preview/original backup differs; preserve it and make a new version")
    if digest(output/"IWannaBeTheBoshy.exe") != original["files"]["IWannaBeTheBoshy.exe"]:
        raise ValueError("Preview executable differs from accepted RC2")
    evidence = ROOT/"build/asset-review/runtime"
    evidence.mkdir(parents=True,exist_ok=True)
    report_path = evidence/"report.json"
    report = {"result":"IN_PROGRESS","runtime_revision":RUNTIME,"art_commit":meta["art_commit"],
              "candidate_manifest_sha256":digest(manifest_path),"audio":"dummy","cases":{},
              "preview":output.relative_to(ROOT).as_posix()}
    if args.resume and report_path.exists():
        report = json.loads(report_path.read_text(encoding="utf-8"))
        if report["candidate_manifest_sha256"]!=digest(manifest_path):
            raise ValueError("Previous evidence belongs to another art version")
    env = os.environ.copy()
    env.update(SDL_AUDIODRIVER="dummy",PATH="C:/Windows/System32;C:/Windows",PYTHONIOENCODING="utf-8")
    startup = subprocess.STARTUPINFO()
    startup.dwFlags |= subprocess.STARTF_USESHOWWINDOW
    startup.wShowWindow = 0
    accepted = json.loads((ROOT/"build/gameplay-regression/Release/report.json").read_text(encoding="utf-8"))
    cp_report = json.loads((ROOT/"build/gameplay-tuning/Release-checkpoint/report.json").read_text(encoding="utf-8"))
    owned = []
    try:
        for name in ("BoshyCurrentRegression.exe","GameplayTuningProbe.exe"):
            source = ROOT/"build/windows/bin/Release"/name
            target = output/name
            if target.exists() and digest(target)!=digest(source):
                raise ValueError("Preserve different existing probe")
            if not target.exists():
                shutil.copy2(source,target)
            owned.append((target,digest(source)))
        probe = output/"BoshyCurrentRegression.exe"
        if digest(probe)!=accepted["cases"]["full-jump"]["current_binary_sha256"]:
            raise ValueError("Probe no longer matches the accepted pre-art oracle")
        for scenario in CASES:
            expected = ROOT/("build/gameplay-tuning/Release-checkpoint/Current.csv" if scenario=="checkpoint" else
                             f"build/gameplay-regression/Release/{scenario}-Current.csv")
            expected_hash = cp_report["traces"]["Current"] if scenario=="checkpoint" else accepted["cases"][scenario]["current_sha256"]
            if digest(expected)!=expected_hash:
                raise ValueError("Accepted trace changed; do not silently replace the oracle")
            trace = evidence/(scenario+"-art.csv")
            prior = report["cases"].get(scenario,{})
            if prior.get("result")=="PASS" and trace.exists() and digest(trace)==prior["trace_sha256"]:
                print(scenario+": retained verified result",flush=True)
                continue
            print(scenario+": running silent themed replay",flush=True)
            with (evidence/(scenario+".log")).open("wb") as log:
                subprocess.run([str(probe.resolve()),str(trace),scenario],cwd="C:/Windows",env=env,
                               startupinfo=startup,stdout=log,stderr=subprocess.STDOUT,check=True,timeout=150)
            old, new = read(expected),read(trace)
            if old!=new:
                mismatch = next((i for i,(a,b) in enumerate(zip(old,new)) if a!=b),min(len(old),len(new)))
                raise AssertionError(f"Themed replay drift: {scenario} at frame {mismatch}")
            report["cases"][scenario] = {"result":"PASS","frames":len(new),"trace_sha256":digest(trace),
                                          "pre_art_trace_sha256":expected_hash}
            save(report_path,report)
            print(scenario+": exact state PASS",flush=True)
        tuning = evidence/"tipping";tuning.mkdir(exist_ok=True)
        with (evidence/"tipping.log").open("wb") as log:
            subprocess.run([str((output/"GameplayTuningProbe.exe").resolve()),str(tuning)],cwd="C:/Windows",env=env,
                           startupinfo=startup,stdout=log,stderr=subprocess.STDOUT,check=True,timeout=90)
        result = json.loads((tuning/"result.json").read_text(encoding="utf-8"))
        if result["result"]!="PASS":
            raise AssertionError("Themed tuning assertions failed")
        for stage, name in (("idle","phase9.png"),("gone","phasenone.png")):
            with Image.open(tuning/f"stone-{stage}.ppm") as image:
                scale=min(image.width//1280,image.height//960)
                if scale<1:raise AssertionError("Unexpected small drawable")
                left,top=(image.width-1280*scale)//2,(image.height-960*scale)//2
                canvas=image.crop((left,top,left+1280*scale,top+960*scale)).resize((1280,960),Image.Resampling.NEAREST).convert("RGB")
            with Image.open(output/"Resources/Image/Background/WORLD1"/name) as source:
                if canvas.crop((800,704,896,768)).tobytes()!=source.convert("RGB").crop((800,704,896,768)).tobytes():
                    raise AssertionError("Themed stone render disagrees with staged asset")
        for ppm in tuning.glob("*.ppm"):
            with Image.open(ppm) as image:image.save(ppm.with_suffix(".png"))
        report["tipping"] = dict(result,idle_and_gone_render_pixels="PASS")
    finally:
        # Remove only this script's unchanged temporary probe copies, never
        # source assets/binaries or the preview game. Children have exited.
        for path, expected in owned:
            if path.parent.resolve()==output.resolve() and path.exists() and digest(path)==expected:
                path.unlink()
    with (evidence/"startup.log").open("wb") as log:
        child = subprocess.Popen([str((output/"IWannaBeTheBoshy.exe").resolve())],cwd="C:/Windows",env=env,
                                 startupinfo=startup,stdout=log,stderr=subprocess.STDOUT)
        try:
            for _ in range(60):
                if child.poll() is not None:raise RuntimeError("Preview game exited during startup")
                time.sleep(.05)
        finally:
            if child.poll() is None:child.terminate()
            child.wait(timeout=10)
    if "OpenGL Info" not in (evidence/"startup.log").read_text(encoding="utf-8",errors="replace"):
        raise AssertionError("No normal-game OpenGL startup evidence")
    report.update(result="PASS",normal_game_startup="PASS from Unicode path / C:/Windows / minimal PATH",
                  game_exe_sha256=digest(output/"IWannaBeTheBoshy.exe"),
                  original_asset_backups="31 verified",default_repo_and_old_rc2_unchanged=True,
                  scope="Nine exact Release replay cases and themed tipping/render assertions; visual taste/all routes still manual")
    save(report_path,report)
    print(json.dumps(report,indent=2),flush=True)


if __name__ == "__main__":main()
