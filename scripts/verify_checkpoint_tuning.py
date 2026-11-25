"""Measure the explicitly authorized counter-shot change against the original.

Does not change the strict parity comparator or relax its expected values.
"""
import argparse
import hashlib
import json
import math
import os
from pathlib import Path
import subprocess
from run_gameplay_regression import read, summarize, coverage

ROOT = Path(__file__).absolute().parents[1]


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--config", choices=["Debug", "Release"], default="Debug")
    args = p.parse_args()
    output = ROOT / "build/gameplay-tuning" / (args.config+"-checkpoint")
    output.mkdir(parents=True, exist_ok=True)
    versions = {}
    startup = subprocess.STARTUPINFO()
    startup.dwFlags |= subprocess.STARTF_USESHOWWINDOW
    startup.wShowWindow = 0
    env = os.environ.copy()
    env.update(SDL_AUDIODRIVER="dummy", PATH="C:/Windows/System32;C:/Windows")
    for name in ("Original", "Current"):
        exe = ROOT / "build/windows/bin" / args.config / f"Boshy{name}Regression.exe"
        trace = output / (name+".csv")
        with (output / (name+".log")).open("wb") as log:
            subprocess.run([str(exe), str(trace), "checkpoint"], cwd="C:/Windows", env=env,
                           startupinfo=startup, stdout=log, stderr=subprocess.STDOUT, check=True, timeout=30)
        rows = read(trace)
        failures = coverage("checkpoint", rows, summarize(rows))
        if failures: raise AssertionError(f"{name} checkpoint coverage: {failures}")
        versions[name] = rows
    old, new = versions["Original"], versions["Current"]
    # Save position, save event, firing update, aiming origin and R stay unchanged.
    first = lambda rows: next(i for i,r in enumerate(rows) if r["cp_bullets"])
    a, b = first(old), first(new)
    if a != b: raise AssertionError("Counter-shot firing delay changed")
    for i, (before, after) in enumerate(zip(old, new)):
        for key in ("cp_x", "cp_y", "cp_room", "cp_world"):
            if key in before and before[key] != after[key]: raise AssertionError(f"Checkpoint {key} changed at {i}")
        if ("checkpoint_saved" in before["events"]) != ("checkpoint_saved" in after["events"]):
            raise AssertionError("Save delay changed")
    parse = lambda row: [float(v) for v in row["cp_bullets"].split(";")[0].split(":")]
    initial_old, initial_new = parse(old[a]), parse(new[b])
    next_old, next_new = parse(old[a+1]), parse(new[b+1])
    delta_old = [next_old[i]-initial_old[i] for i in (0,1)]
    delta_new = [next_new[i]-initial_new[i] for i in (0,1)]
    speed_old, speed_new = math.hypot(*delta_old), math.hypot(*delta_new)
    if abs(speed_old-8)>.0001 or abs(speed_new-16)>.0001:
        raise AssertionError(f"Unexpected speeds: {speed_old}, {speed_new}")
    if any(abs(delta_new[i]-2*delta_old[i])>.0001 for i in (0,1)):
        raise AssertionError("Aiming direction changed")
    if initial_old[3] != initial_new[3]: raise AssertionError("Projectile lifetime changed")
    hit = lambda rows: next(i for i,r in enumerate(rows) if r["death"]=="1")
    if hit(new) >= hit(old): raise AssertionError("Doubled shot did not hit sooner in this unobstructed fixture")
    report = {"result":"PASS", "config":args.config, "audio":"dummy", "scope":"Explicit user-approved speed exception; not original gameplay parity",
              "firing_frame":a, "speed_before":speed_old, "speed_after":speed_new,
              "hit_frame_before":hit(old), "hit_frame_after":hit(new),
              "save_delay_and_R":"PASS", "aiming_and_lifetime":"PASS",
              "traces":{n:hashlib.sha256((output/(n+".csv")).read_bytes()).hexdigest() for n in versions}}
    (output / "report.json").write_text(json.dumps(report,indent=2)+"\n",encoding="utf-8")
    print(json.dumps(report,indent=2),flush=True)


if __name__ == "__main__": main()
