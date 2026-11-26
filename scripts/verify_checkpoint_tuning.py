"""Measure checkpoint hit-to-shot reaction against the original.

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
    p.add_argument("--current-directory", type=Path, help="Optional isolated current probe/runtime directory")
    p.add_argument("--output", type=Path, help="Preserve separate evidence when testing a themed runtime")
    args = p.parse_args()
    output = args.output or ROOT / "build/gameplay-tuning" / (args.config+"-checkpoint-reaction")
    output.mkdir(parents=True, exist_ok=True)
    versions = {}
    binaries = {}
    startup = subprocess.STARTUPINFO()
    startup.dwFlags |= subprocess.STARTF_USESHOWWINDOW
    startup.wShowWindow = 0
    env = os.environ.copy()
    env.update(SDL_AUDIODRIVER="dummy", PATH="C:/Windows/System32;C:/Windows")
    for name in ("Original", "Current"):
        directory = args.current_directory if name == "Current" and args.current_directory else ROOT / "build/windows/bin" / args.config
        exe = directory / f"Boshy{name}Regression.exe"
        binaries[name] = hashlib.sha256(exe.read_bytes()).hexdigest()
        trace = output / (name+".csv")
        with (output / (name+".log")).open("wb") as log:
            subprocess.run([str(exe), str(trace), "checkpoint"], cwd="C:/Windows", env=env,
                           startupinfo=startup, stdout=log, stderr=subprocess.STDOUT, check=True, timeout=30)
        rows = read(trace)
        failures = coverage("checkpoint", rows, summarize(rows))
        if failures: raise AssertionError(f"{name} checkpoint coverage: {failures}")
        versions[name] = rows
    old, new = versions["Original"], versions["Current"]
    if len(old) != len(new): raise AssertionError("Trace lengths differ")
    # Activation/save timing and position, aiming and R stay unchanged.
    first = lambda rows: next(i for i,r in enumerate(rows) if r["cp_bullets"])
    a, b = first(old), first(new)
    activation = lambda rows: next(i for i,r in enumerate(rows) if "checkpoint_activated" in r["events"])
    activated_old, activated_new = activation(old), activation(new)
    if activated_old != activated_new: raise AssertionError("Checkpoint activation changed")
    # With 1/60 float updates, original 1.0 crosses after61 additions; 0.5 after30.
    # Assert those exact observed update counts, without relaxing state parity.
    if (a-activated_old, b-activated_new) != (61, 30):
        raise AssertionError(f"Unexpected hit-to-shot updates: {a-activated_old}, {b-activated_new}")
    for i, (before, after) in enumerate(zip(old, new)):
        for key in ("cp_x", "cp_y", "cp_room", "cp_world"):
            if key in before and before[key] != after[key]: raise AssertionError(f"Checkpoint {key} changed at {i}")
        if ("checkpoint_saved" in before["events"]) != ("checkpoint_saved" in after["events"]):
            raise AssertionError("Save delay changed")
        if ("checkpoint_activated" in before["events"]) != ("checkpoint_activated" in after["events"]):
            raise AssertionError("Checkpoint hit sequence changed")
        if ("respawn" in before["events"]) != ("respawn" in after["events"]):
            raise AssertionError("Respawn timing changed")
    parse = lambda row: [float(v) for v in row["cp_bullets"].split(";")[0].split(":")]
    initial_old, initial_new = parse(old[a]), parse(new[b])
    next_old, next_new = parse(old[a+1]), parse(new[b+1])
    delta_old = [next_old[i]-initial_old[i] for i in (0,1)]
    delta_new = [next_new[i]-initial_new[i] for i in (0,1)]
    speed_old, speed_new = math.hypot(*delta_old), math.hypot(*delta_new)
    if abs(speed_old-8)>.0001 or abs(speed_new-8)>.0001:
        raise AssertionError(f"Unexpected speeds: {speed_old}, {speed_new}")
    if any(abs(delta_new[i]-delta_old[i])>.0001 for i in (0,1)):
        raise AssertionError("Aiming direction changed")
    if initial_old[3] != initial_new[3]: raise AssertionError("Projectile lifetime changed")
    hit = lambda rows: next(i for i,r in enumerate(rows) if r["death"]=="1")
    if hit(new) >= hit(old): raise AssertionError("Faster reaction did not hit sooner in this unobstructed fixture")
    report = {"result":"PASS", "config":args.config, "audio":"dummy", "scope":"User-corrected reaction delay; projectile travel restored to original. Not original timing parity.",
              "activation_frame":activated_old, "firing_frame_before":a, "firing_frame_after":b,
              "reaction_updates_before":a-activated_old, "reaction_updates_after":b-activated_new,
              "delay_threshold_before":1.0, "delay_threshold_after":0.5,
              "speed_before":speed_old, "speed_after":speed_new,
              "hit_frame_before":hit(old), "hit_frame_after":hit(new),
              "save_delay_and_R":"PASS", "aiming_and_lifetime":"PASS",
              "binary_sha256":binaries,
              "traces":{n:hashlib.sha256((output/(n+".csv")).read_bytes()).hexdigest() for n in versions}}
    (output / "report.json").write_text(json.dumps(report,indent=2)+"\n",encoding="utf-8")
    print(json.dumps(report,indent=2),flush=True)


if __name__ == "__main__": main()
