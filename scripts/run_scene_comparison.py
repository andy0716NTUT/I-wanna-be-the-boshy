"""Build and run opt-in original/Phase-1 graphics probes, then compare traces."""
import argparse
import os
from pathlib import Path
import subprocess
import sys


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cmake", type=Path, required=True)
    parser.add_argument("--build-dir", type=Path, default=Path("build/windows"))
    parser.add_argument("--config", choices=["Debug", "Release"], default="Debug")
    args = parser.parse_args()
    root = Path(__file__).absolute().parents[1]
    build = (root / args.build_dir).absolute()
    output = root / "build/baseline" / args.config
    output.mkdir(parents=True, exist_ok=True)
    env = os.environ.copy()

    def run(command, log, runtime=False):
        print(subprocess.list2cmdline([str(x) for x in command]), flush=True)
        process_env = env.copy()
        options = {}
        if runtime and os.name == "nt":
            windows = Path(env.get("SystemRoot", "C:/Windows"))
            process_env["PATH"] = os.pathsep.join([str(windows / "System32"), str(windows)])
            startup = subprocess.STARTUPINFO()
            startup.dwFlags |= subprocess.STARTF_USESHOWWINDOW
            startup.wShowWindow = 0
            options["startupinfo"] = startup
        if runtime:
            process_env["SDL_AUDIODRIVER"] = "dummy"  # Test processes only; keep shipping sound defaults.
        with log.open("wb") as stream:
            subprocess.run([str(x) for x in command], cwd=root, env=process_env,
                           stdout=stream, stderr=subprocess.STDOUT, check=True,
                           timeout=20 if runtime else 300, **options)

    run([sys.executable, root / "scripts/prepare_original_probe.py"], output / "prepare.log")
    # Use the already verified build cache; do not infer or install another toolchain.
    run([args.cmake, "-S", root, "-B", build, "-DBOSHY_BUILD_SCENE_PROBES=ON",
         f"-DBOSHY_ORIGINAL_SOURCE_DIR:PATH={(root / 'build/original-probe-source').as_posix()}"], output / "configure.log")
    run([args.cmake, "--build", build, "--config", args.config, "--target",
         "BoshyOriginalProbe", "BoshyPhase1Probe", "--parallel", "8"], output / "build.log")
    for version in ["Original", "Phase1"]:
        run([build / "bin" / args.config / f"Boshy{version}Probe.exe", output / f"{version}.csv"],
            output / f"{version}.log", runtime=True)
    report = output / "comparison.json"
    subprocess.run([sys.executable, root / "scripts/compare_scene_traces.py",
                    output / "Original.csv", output / "Phase1.csv", "--report", report],
                   cwd=root, check=True)
    print(f"Comparison report: {report}")


if __name__ == "__main__":
    main()
