"""Measure original/current game events using the real renderer and update paths."""
import argparse
from collections import Counter
import csv
import hashlib
import json
import os
from pathlib import Path
import subprocess
import sys
import time

ROOT = Path(__file__).absolute().parents[1]
SCENARIOS = ["full-jump", "short-jump", "double-jump", "movement", "wall", "head", "oneway",
             "shooting", "shoot-jump", "spikes", "checkpoint", "transition", "enemy", "boss", "boss-death", "death-loop", "reload-stress", "spider-bird"]


def read(path):
    with path.open(encoding="utf-8", newline="") as stream:
        return list(csv.DictReader(stream, delimiter="|"))


def summarize(rows):
    events = Counter(event for row in rows for event in row["events"].split(",") if event)
    shots = [int(row["frame"]) for row in rows if "shot" in row["events"].split(",")]
    jumps = [int(row["frame"]) for row in rows if "jump" in row["events"].split(",")]
    start_y = float(rows[9]["y"])
    apex = max(rows[10:], key=lambda row: float(row["y"]))
    land = next((int(row["frame"]) for row in rows[11:]
                 if "floor" in row["events"].split(",") and float(row["y"]) <= start_y), None)
    return {"frames": len(rows), "events": dict(events), "shot_frames": shots,
            "shot_intervals": sorted(set(b-a for a,b in zip(shots,shots[1:]))),
            "max_bullets": max(len([b for b in r["bullets"].split(";") if b]) for r in rows),
            "jump_frames": jumps, "max_jump_count": max(int(r["jumps"]) for r in rows),
            "start_y": start_y, "apex_y": float(apex["y"]), "apex_frame": int(apex["frame"]),
            "rise": float(apex["y"])-start_y, "landing_frame": land,
            "airborne_updates": land-jumps[0] if land is not None and jumps else None,
            "death_frames": sum(r["death"] == "1" for r in rows),
            "ui_visible_frames": sum(r["death_ui"] == "1" for r in rows),
            "rooms": sorted(set(r["loaded_room"] for r in rows)),
            "boss_attacks": sorted(set(int(r["boss"].split(":")[0]) for r in rows if r["boss"] != "none")),
            "min_boss_hp": min([int(r["boss"].split(":")[1]) for r in rows if r["boss"] != "none"] or [0]),
            "bear_frames": sum(r["bear"] != "none" for r in rows)}


def coverage(scenario, rows, metrics):
    failures = []
    events = metrics["events"]
    def need(condition, reason):
        if not condition: failures.append(reason)
    need(len(rows) == (3300 if scenario == "boss" else 1200 if scenario == "boss-death" else 300 if scenario == "enemy" else 450 if scenario == "death-loop" else 600 if scenario == "reload-stress" else 900 if scenario == "spider-bird" else 150), "Incomplete trace")
    if scenario in ("full-jump", "short-jump", "double-jump"):
        need(float(rows[9]["vy"]) == 0 and int(rows[9]["jumps"]) == 0, "Jump did not begin grounded")
        need(metrics["rise"] > 0 and metrics["landing_frame"] is not None, "No observed apex and ground landing")
        need(events.get("jump",0) == (2 if scenario == "double-jump" else 1), "Wrong number of accepted jump inputs")
        if scenario == "short-jump": need(events.get("shorten_jump",0) == 1, "Early release not exercised")
        if scenario == "double-jump": need(metrics["max_jump_count"] == 2, "Second-jump limit not exercised")
    if scenario == "movement":
        need(float(rows[19]["x"])-float(rows[9]["x"]) == 50, "Right travel interval not exercised")
        need(float(rows[39]["x"])-float(rows[29]["x"]) == -50, "Left travel interval not exercised")
        need(rows[49]["x"] == rows[59]["x"], "Simultaneous left/right convention not exercised")
        need(float(rows[79]["y"])-float(rows[69]["y"]) == 5 and
             float(rows[99]["y"])-float(rows[89]["y"]) == -5, "Existing up/down controls not exercised")
        if "input" in rows[0]:
            need(sum(row['input'][0]=='1' for row in rows) == 20 and
                 sum(row['input'][1]=='1' for row in rows) == 20, "Movement keys not received")
    if scenario == "wall":
        need(len(set(row["x"] for row in rows[:40])) == 1, "Blocked wall movement drifted")
        if "input" in rows[0]: need(all(row['input'][0]=='1' for row in rows[:40]), "Blocked Right key not held")
    if scenario == "head": need(events.get("head",0) > 0, "No rising head collision")
    if scenario == "oneway":
        need(events.get("floor",0) > 0 and "floor" in rows[50]["events"].split(","), "No one-way landing")
        need(float(rows[5]["vy"]) > 0 and "head" not in rows[5]["events"], "Pass through from below not exercised")
    if scenario in ("shooting", "shoot-jump"):
        need(len(metrics["shot_frames"]) >= 5 and metrics["max_bullets"] == 5, "Bullet cap/cadence not reached")
        if scenario == "shoot-jump": need("shot" in rows[10]["events"] and "jump" not in rows[10]["events"], "X+Z precedence not exercised")
    if scenario == "spikes":
        need(events.get("spike",0) > 0 and metrics["death_frames"] > 45, "Spike/death duration not exercised")
        need(metrics["ui_visible_frames"] > 45 and rows[1]["player_visible"] == "0", "Death UI/player visibility not observed")
        need("respawn" in rows[90]["events"] and rows[90]["death_ui"] == "0", "R did not hide death UI")
    if scenario == "death-loop":
        need(events.get("respawn",0) == 3 and metrics["ui_visible_frames"] > 135, "Repeated death/R lifecycle not exercised")
        need(all(rows[i]["death_ui"] == "0" for i in (90,240,390)), "Repeated R did not hide UI")
    if scenario == "checkpoint":
        need(events.get("checkpoint_saved",0) > 0, "No bullet-activated save")
        need(any(row["cp_bullets"] for row in rows), "Checkpoint counter-shot not observed")
        need("respawn" in rows[110]["events"] and rows[110]["x"] == rows[110]["cp_x"] and
             rows[110]["y"] == rows[110]["cp_y"], "R did not restore captured checkpoint")
    if scenario == "transition": need(events.get("room_transition",0) >= 2 and metrics["rooms"] == ["1","2"], "Room exit/re-entry not covered")
    if scenario == "enemy": need(metrics["bear_frames"] > 100, "Bear encounter not exercised")
    if scenario == "boss": need(set(metrics["boss_attacks"]) >= {0,1,2,3,4}, "Full Boss attack cycle not exercised")
    if scenario == "reload-stress":
        need(metrics["rooms"] == ["1","2"], "Repeated room reload not exercised")
    if scenario == "spider-bird":
        need(metrics["rooms"] == ["12","8","9"], "Spider/bird room re-entry not exercised")
        states = {int(r["spider"].split(":")[0]) for r in rows if r["spider"] != "none"}
        need(states >= {1,2,3,4,5}, "Full spider cycle not exercised")
        need(sum(r["bird"] != "none" for r in rows) > 100, "Bird chase not exercised")
        need(rows[450]["spider"] != "none", "Original immediate re-entry timer convention not observed")
    if scenario == "boss-death":
        need(metrics['min_boss_hp'] == 0, "Boss damage/defeat not observed")
        need(len(set(row['map_hash'] for row in rows[450:])) > 1, "Victory platform extension not observed")
    return failures


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cmake", type=Path, required=True)
    parser.add_argument("--config", choices=["Debug", "Release"], default="Debug")
    parser.add_argument("--scenarios", nargs="+", choices=SCENARIOS, default=SCENARIOS)
    parser.add_argument("--no-build", action="store_true")
    parser.add_argument("--fps-cap", type=int, choices=[30,60,120], help="Test-process frame cap; leaves deployed config unchanged")
    args = parser.parse_args()
    build = ROOT / "build/windows"
    output = ROOT / "build/gameplay-regression" / (args.config + (f"-fps{args.fps_cap}" if args.fps_cap else ""))
    output.mkdir(parents=True, exist_ok=True)
    if not args.no_build:
        subprocess.run([sys.executable, ROOT / "scripts/prepare_regression_sources.py"], check=True)
        for command in ([args.cmake,"-S",ROOT,"-B",build,"-DBOSHY_BUILD_REGRESSION_PROBES=ON"],
                        [args.cmake,"--build",build,"--config",args.config,"--target",
                         "BoshyOriginalRegression","BoshyCurrentRegression","--parallel","8"]):
            with (output / "build.log").open("ab") as log:
                subprocess.run([str(x) for x in command], stdout=log, stderr=subprocess.STDOUT, check=True, cwd=ROOT)
    report = {"git_head": subprocess.check_output(["git","rev-parse","HEAD"],cwd=ROOT).decode().strip(),
              "config": args.config, "fps_cap_override": args.fps_cap, "cases": {}, "result": "IN_PROGRESS"}
    report_path = output / "report.json"
    for scenario in args.scenarios:
        result = {"parity": "NOT_RUN", "coverage": "NOT_RUN"}
        rows = {}
        for version in ("Original", "Current"):
            target = build / "bin" / args.config / f"Boshy{version}Regression.exe"
            trace = output / f"{scenario}-{version}.csv"
            env = os.environ.copy()
            windows = Path(env.get("SystemRoot","C:/Windows"))
            env["PATH"] = os.pathsep.join([str(windows/"System32"),str(windows)])
            if args.fps_cap:
                env["BOSHY_PROBE_FPS_CAP"] = str(args.fps_cap)
            env["SDL_AUDIODRIVER"] = "dummy"  # User preference: testing must remain silent.
            startup = subprocess.STARTUPINFO()
            startup.dwFlags |= subprocess.STARTF_USESHOWWINDOW
            startup.wShowWindow = 0
            started = time.perf_counter()
            with (output / f"{scenario}-{version}.log").open("wb") as log:
                run = subprocess.run([str(target),str(trace),scenario], cwd=windows, env=env,
                                     stdout=log, stderr=subprocess.STDOUT, startupinfo=startup, timeout=150)
            result[version.lower()+"_elapsed_seconds"] = round(time.perf_counter()-started,4)
            result[version.lower()+"_binary_sha256"] = hashlib.sha256(target.read_bytes()).hexdigest()
            result[version.lower()+"_manifest_sha256"] = hashlib.sha256((ROOT / "build/regression-source" / version / "manifest.json").read_bytes()).hexdigest()
            if run.returncode:
                result["error"] = f"{version} probe exited {run.returncode}; inspect its log"
                break
            rows[version] = read(trace)
            result[version.lower()+"_sha256"] = hashlib.sha256(trace.read_bytes()).hexdigest()
        if len(rows) == 2:
            differences = [{"frame":i,"fields":[k for k in left if left[k] != right.get(k)]}
                           for i,(left,right) in enumerate(zip(rows["Original"],rows["Current"])) if left != right]
            result["parity"] = "PASS" if not differences and len(rows["Original"]) == len(rows["Current"]) else "FAIL"
            result["differences"] = differences[:10]
            result["metrics"] = summarize(rows["Original"])
            failures = coverage(scenario,rows["Original"],result["metrics"])
            result["coverage"] = "PASS" if not failures else "FAIL"
            result["coverage_failures"] = failures
        report["cases"][scenario] = result
        report_path.write_text(json.dumps(report,indent=2)+"\n",encoding="utf-8")
        print(f"{scenario}: parity={result['parity']} coverage={result['coverage']} {result.get('error','')} {result.get('coverage_failures',[])}",flush=True)
    report["result"] = "PASS" if all(c["parity"] == c["coverage"] == "PASS" for c in report["cases"].values()) else "FAIL"
    report_path.write_text(json.dumps(report,indent=2)+"\n",encoding="utf-8")
    print(f"{report['result']}: {report_path}",flush=True)
    return 0 if report["result"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
