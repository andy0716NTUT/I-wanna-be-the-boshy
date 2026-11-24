"""Compare scripted App traces exactly, without claiming unobserved gameplay paths."""
import argparse
import csv
import json
from pathlib import Path
import re


def read(path):
    with path.open(encoding="utf-8", newline="") as stream:
        return list(csv.DictReader(stream, delimiter="|"))


def position(row):
    match = re.fullmatch(r"Position: \(([-\d.e+]+), ([-\d.e+]+)\)", row["position_before_update"])
    return tuple(map(float, match.groups())) if match else None


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("original", type=Path)
    parser.add_argument("phase1", type=Path)
    parser.add_argument("--report", type=Path, required=True)
    args = parser.parse_args()
    original, phase1 = read(args.original), read(args.phase1)
    def state(row):
        return {key: value for key, value in row.items() if key != "elapsed_ms"}
    differences = [i for i, (left, right) in enumerate(zip(original, phase1)) if state(left) != state(right)]
    failures = []
    if len(original) != 320 or len(phase1) != 320:
        failures.append("Expected 320 actual App updates per executable")
    if differences or len(original) != len(phase1):
        failures.append("Logged frame state differs; inspect raw traces before changing gameplay")
    expected_inputs = {"right": 10, "left": 10, "z": 23, "x": 60, "r": 2}
    counts = {key: sum(row[key] == "1" for row in phase1) for key in expected_inputs}
    if counts != expected_inputs:
        failures.append("Input polling did not receive the scheduled held-key frames")
    entered = [int(row["frame"]) for row in phase1 if row["phase"] != "0"]
    if not entered:
        failures.append("Menu entry did not run")
    samples = {}
    timing = {}
    for version, rows in [("original", original), ("phase1", phase1)]:
        times = [float(row["elapsed_ms"]) for row in rows]
        intervals = [right - left for left, right in zip(times, times[1:])]
        if intervals:
            timing[version] = {"elapsed_first_to_last_ms": times[-1] - times[0],
                               "mean_update_interval_ms": sum(intervals) / len(intervals),
                               "min_update_interval_ms": min(intervals),
                               "max_update_interval_ms": max(intervals)}
    if len(phase1) == 320:
        before, after = position(phase1[51]), position(phase1[61])
        if before and after:
            samples["right_10_updates_displacement"] = [after[i] - before[i] for i in range(2)]
        for label, begin, end in [("held_jump_window", 81, 139), ("short_jump_window", 141, 179)]:
            points = [(int(row["frame"]), position(row)) for row in phase1[begin:end + 1]]
            points = [(frame, point) for frame, point in points if point is not None]
            if points:
                apex = max(points, key=lambda point: point[1][1])
                samples[label] = {"first_frame": points[0][0], "start_y": points[0][1][1],
                                  "max_logged_y": apex[1][1], "max_y_frame": apex[0],
                                  "rise_from_first_sample": apex[1][1] - points[0][1][1]}
    report = {"result": "FAIL" if failures else "PASS", "original_rows": len(original),
              "phase1_rows": len(phase1), "differing_frames": differences,
              "observed_input_frames": counts, "first_non_menu_frame": min(entered) if entered else None,
              "coordinate_samples": samples, "timing_observations": timing, "failures": failures,
              "limits": ["Exact equality of existing DebugInfo decimal strings, not full internal state.",
                         "Animation uses real wall time; no visual or animation timing parity assertion.",
                         "Death state, velocity, active bullet count, checkpoint and boss state are not exposed.",
                         "Coordinate peaks are observations, not proof of a collision-free jump or landing.",
                         "Wall-time intervals are observed separately; not an FPS stress/performance gate.",
                         "A PASS covers this input workload only; Stage 1.5 gate remains separate."]}
    args.report.parent.mkdir(parents=True, exist_ok=True)
    args.report.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(report, indent=2))
    return 1 if failures else 0


if __name__ == "__main__":
    raise SystemExit(main())
