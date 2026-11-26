"""Verify the opt-in weighted feel against fresh Original-profile game traces.

Does not relax the original/current parity comparator. All gameplay runs silent.
"""
import argparse
import hashlib
import json
import math
import os
from pathlib import Path
import shutil
import subprocess
import time
from PIL import Image

from run_gameplay_regression import read, summarize, coverage

ROOT = Path(__file__).absolute().parents[1]
CASES = ["full-jump", "short-jump", "double-jump", "fall", "movement", "wall", "head", "oneway",
         "shooting", "spikes", "checkpoint", "transition", "death-loop", "resize-play"]


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--config", choices=["Debug", "Release"], default="Debug")
    parser.add_argument("--runtime-directory", type=Path, help="Optional same-code scenery runtime")
    parser.add_argument("--output", type=Path)
    parser.add_argument("--scenarios", nargs="+", choices=CASES, default=CASES)
    args = parser.parse_args()
    binaries = ROOT/"build/windows/bin"/args.config
    source = args.runtime_directory or binaries
    if digest(source/"IWannaBeTheBoshy.exe") != digest(binaries/"IWannaBeTheBoshy.exe"):
        raise ValueError("Runtime and freshly built probes must use the same normal executable")
    output = args.output or ROOT/"build/player-feel-v1"/args.config
    output = output.absolute()
    output.resolve().relative_to((ROOT/"build").resolve())
    if output.exists():
        raise FileExistsError("Preserve existing results/preview; select a new --output")
    output.mkdir(parents=True)
    stage = output/"Weighted"
    stage.mkdir()
    for name in ("Resources", "PTSDAssets", "OriginalAssets"):
        if (source/name).is_dir(): shutil.copytree(source/name, stage/name)
    for name in ("art-preview.json", "baseline-manifest.json"):
        if (source/name).is_file(): shutil.copy2(source/name,stage/name)
    for file in source.glob("*.dll"):
        shutil.copy2(file,stage/file.name)
    shutil.copy2(source/"IWannaBeTheBoshy.exe",stage/"IWannaBeTheBoshy.exe")
    config_bytes = (source/"config.json").read_bytes()
    config = json.loads(config_bytes)
    if config.get("player_feel", "original") != "original":
        raise ValueError("Require Original-profile base, never silently override an existing preview")
    (stage/"config-original.json").write_bytes(config_bytes)
    config["player_feel"] = "weighted"
    (stage/"config.json").write_text(json.dumps(config,indent=2)+"\n",encoding="utf-8")
    (stage/"Play-Weighted-Silent.cmd").write_bytes(
        b'@echo off\r\nsetlocal\r\nset "SDL_AUDIODRIVER=dummy"\r\n"%~dp0IWannaBeTheBoshy.exe"\r\nendlocal\r\n')
    original_exe = os.path.relpath(source/"IWannaBeTheBoshy.exe",stage).replace("/","\\")
    (stage/"Play-Original-Silent.cmd").write_bytes(
        ('@echo off\r\nsetlocal\r\nset "SDL_AUDIODRIVER=dummy"\r\n"%~dp0'+original_exe+'"\r\nendlocal\r\n').encode("ascii"))
    (stage/"README-Player-Feel.txt").write_text(
        "Optional weighted falling preview; Original remains the normal default.\n"
        "Play-Weighted-Silent.cmd starts this preview without sound.\n"
        "Play-Original-Silent.cmd opens the retained same-code base with Original feel.\n"
        "Keep that base folder in place for the comparison launcher.\n"
        "Debug Window > Controls > Player feel switches profiles immediately.\n"
        "Restart reads config.json again; config-original.json preserves the base choice.\n"
        "Jump rise is unchanged, falling accelerates faster and caps at14 rather than10.\n"
        "Earlier landing shortens gap reach; this candidate still needs human playtesting.\n"
        "Source images and old releases are retained; no public release is implied.\n",
        encoding="utf-8")
    accepted_path = ROOT/"build/gameplay-regression"/args.config/"report.json"
    accepted = json.loads(accepted_path.read_text())
    checkpoint = ROOT/"build/gameplay-tuning"/(args.config+"-checkpoint-reaction")
    checkpoint_report = json.loads((checkpoint/"report.json").read_text())
    if accepted["result"] != "PASS" or checkpoint_report["result"] != "PASS":
        raise ValueError("Require fresh passing Original-profile oracle reports")
    report = {"result":"IN_PROGRESS", "config":args.config, "audio":"dummy", "profile":"weighted",
              "scope":"Intentional falling-only physics experiment; not original gameplay parity or human feel approval",
              "git_head_during_test":subprocess.check_output(["git","rev-parse","HEAD"],cwd=ROOT,text=True).strip(),
              "current_source_manifest_sha256":digest(ROOT/"build/regression-source/Current/manifest.json"),
              "game_sha256":digest(stage/"IWannaBeTheBoshy.exe"), "cases":{},
              "preview":stage.relative_to(ROOT).as_posix(), "base":source.relative_to(ROOT).as_posix()}
    startup = subprocess.STARTUPINFO()
    startup.dwFlags |= subprocess.STARTF_USESHOWWINDOW
    startup.wShowWindow = 0
    env = os.environ.copy()
    env.update(SDL_AUDIODRIVER="dummy", PATH="C:/Windows/System32;C:/Windows")
    owned = []
    try:
        for name in ("BoshyCurrentRegression.exe", "GameplayTuningProbe.exe"):
            target = stage/name
            shutil.copy2(binaries/name,target)
            owned.append((target,digest(target)))
        probe = stage/"BoshyCurrentRegression.exe"
        for scenario in args.scenarios:
            if scenario == "checkpoint":
                baseline = checkpoint/"Current.csv"
                expected_hash = checkpoint_report["traces"]["Current"]
                expected_binary = checkpoint_report["binary_sha256"]["Current"]
            else:
                baseline = accepted_path.parent/(scenario+"-Current.csv")
                expected_hash = accepted["cases"][scenario]["current_sha256"]
                expected_binary = accepted["cases"][scenario]["current_binary_sha256"]
            if digest(baseline) != expected_hash or digest(probe) != expected_binary:
                raise ValueError("Oracle/probe changed; rerun Original-profile validation")
            trace = output/(scenario+"-weighted.csv")
            with (output/(scenario+".log")).open("wb") as log:
                subprocess.run([str(probe.resolve()),str(trace),scenario],cwd="C:/Windows",env=env,
                               startupinfo=startup,stdout=log,stderr=subprocess.STDOUT,check=True,timeout=90)
            old, new = read(baseline), read(trace)
            before, after = summarize(old), summarize(new)
            failures = coverage(scenario,new,after) if scenario != "movement" else []
            if len(new)!=len(old): failures.append("Incomplete replay")
            if min(float(r["vy"]) for r in new) < -14: failures.append("Exceeded14-unit terminal velocity")
            if scenario in ("full-jump","short-jump"):
                apex = before["apex_frame"]
                if any(old[i][k]!=new[i][k] for i in range(10,apex+1) for k in ("x","y","vy","jumps")):
                    failures.append("Original ascent changed")
                if after["rise"]!=before["rise"] or after["landing_frame"]>=before["landing_frame"]:
                    failures.append("Expected same jump height and earlier landing")
            if scenario == "fall":
                if min(float(r["vy"]) for r in new)!=-14: failures.append("14-unit fall not observed")
                landing = lambda rows: next(int(r["frame"]) for r in rows[11:] if "floor" in r["events"].split(","))
                if landing(new)>=landing(old) or new[-1]["y"]!=old[-1]["y"]:
                    failures.append("Fall did not land sooner on the same floor")
            if scenario == "movement" and any(a["x"]!=b["x"] or a["input"]!=b["input"] for a,b in zip(old,new)):
                failures.append("Horizontal movement/input changed")
            if scenario == "shooting" and any(before[k]!=after[k] for k in ("shot_frames","shot_intervals","max_bullets")):
                failures.append("Player shooting cadence/cap changed")
            if scenario == "checkpoint":
                activated = next(int(r["frame"]) for r in new if "checkpoint_activated" in r["events"].split(","))
                shot = next(i for i,r in enumerate(new) if r["cp_bullets"])
                parse = lambda r: [float(v) for v in r["cp_bullets"].split(";")[0].split(":")]
                a,b = parse(new[shot]),parse(new[shot+1])
                if shot-activated!=30 or abs(math.hypot(b[0]-a[0],b[1]-a[1])-8)>.0001:
                    failures.append("Checkpoint reaction/travel changed")
            report["cases"][scenario] = {"result":"FAIL" if failures else "PASS", "failures":failures,
                                          "original":before,"weighted":after,"trace_sha256":digest(trace),
                                          "baseline_trace_sha256":expected_hash,"probe_sha256":digest(probe)}
            (output/"report.json").write_text(json.dumps(report,indent=2)+"\n",encoding="utf-8")
            print(f"{scenario}: {report['cases'][scenario]['result']} {failures}",flush=True)
            if failures: raise AssertionError(f"{scenario}: {failures}")
        tuning = output/"tuning"
        tuning.mkdir()
        with (output/"tuning.log").open("wb") as log:
            subprocess.run([str((stage/"GameplayTuningProbe.exe").resolve()),str(tuning)],cwd="C:/Windows",env=env,
                           startupinfo=startup,stdout=log,stderr=subprocess.STDOUT,check=True,timeout=90)
        report["tuning"] = json.loads((tuning/"result.json").read_text())
        if report["tuning"]["result"]!="PASS": raise AssertionError("Weighted tuning assertions failed")
        for stage_name, image_name in (("idle","phase9.png"),("gone","phasenone.png")):
            with Image.open(tuning/f"stone-{stage_name}.ppm") as frame, Image.open(stage/"Resources/Image/Background/WORLD1"/image_name) as image:
                if frame.size!=(1280,960): raise AssertionError("Unexpected native drawable")
                if frame.convert("RGB").crop((800,704,896,768)).tobytes()!=image.convert("RGB").crop((800,704,896,768)).tobytes():
                    raise AssertionError("Weighted stone/cover pixels differ from staged art")
        report["tuning"]["idle_and_gone_pixels"] = "PASS"
    finally:
        # Remove only the unchanged probes this invocation copied, after child exit.
        for path, expected in owned:
            if path.parent.resolve()==stage.resolve() and path.exists() and digest(path)==expected:
                path.unlink()
    startup_log = output/"startup.log"
    with startup_log.open("wb") as log:
        child = subprocess.Popen([str((stage/"IWannaBeTheBoshy.exe").resolve())],cwd="C:/Windows",env=env,
                                 startupinfo=startup,stdout=log,stderr=subprocess.STDOUT)
        try:
            for _ in range(60):
                if child.poll() is not None: raise RuntimeError("Weighted normal game exited during startup")
                time.sleep(.05)
        finally:
            if child.poll() is None: child.terminate()
            child.wait(timeout=10)
    startup_text = startup_log.read_text(encoding="utf-8",errors="replace")
    if "OpenGL Info" not in startup_text or "Boshy failed:" in startup_text or "Boshy startup failed:" in startup_text:
        raise AssertionError("Weighted normal-game startup failed")
    report["normal_game_startup"] = "PASS: Unicode path, unrelated cwd, minimal PATH, dummy audio; runner stopped owned startup-only process"
    report["verified_copied_files"] = {}
    for directory in ("Resources", "OriginalAssets"):
        files = [p for p in (source/directory).rglob("*") if p.is_file()]
        for file in files:
            if digest(file)!=digest(stage/directory/file.relative_to(source/directory)):
                raise AssertionError("Staged resource/original backup differs from base")
        report["verified_copied_files"][directory] = len(files)
    report["result"] = "PASS"
    report["config_original_sha256"] = hashlib.sha256(config_bytes).hexdigest()
    report["config_weighted_sha256"] = digest(stage/"config.json")
    (output/"report.json").write_text(json.dumps(report,indent=2)+"\n",encoding="utf-8")
    print("PASS: "+str(output/"report.json"),flush=True)


if __name__ == "__main__": main()
