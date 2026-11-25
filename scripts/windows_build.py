"""Configure, build and run CTest without changing the system environment."""
import argparse
import os
from pathlib import Path
import shutil
import subprocess


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cmake", type=Path, required=True)
    parser.add_argument("--toolchain", type=Path, required=True,
                        help="Directory containing gcc.exe, g++.exe and windres.exe")
    parser.add_argument("--ninja", type=Path, required=True)
    parser.add_argument("--build-dir", type=Path, default=Path("build/windows"))
    parser.add_argument("--config", choices=["Debug", "Release"], default="Debug")
    parser.add_argument("--offline", action="store_true",
                        help="Only after all pinned FetchContent sources have been populated")
    parser.add_argument("--jobs", type=int, default=8)
    args = parser.parse_args()
    root = Path(__file__).absolute().parents[1]  # Preserve an ASCII subst alias.
    build = (root / args.build_dir).absolute()
    build.mkdir(parents=True, exist_ok=True)
    cmake = args.cmake.absolute()
    toolchain = args.toolchain.absolute()
    ninja = args.ninja.absolute()
    git = shutil.which("git")
    program_dirs = [str(toolchain), str(ninja.parent)]
    if git:
        program_dirs.append(str(Path(git).parent))
    env = os.environ.copy()
    env["SDL_AUDIODRIVER"] = "dummy"  # Build/test invocation only; never alter shipping config.
    env["PATH"] = os.pathsep.join(program_dirs + [env.get("PATH", "")])
    configure = [str(cmake), "-S", str(root), "-B", str(build), "-G", "Ninja Multi-Config",
                 f"-DCMAKE_C_COMPILER={(toolchain / 'gcc.exe').as_posix()}",
                 f"-DCMAKE_CXX_COMPILER={(toolchain / 'g++.exe').as_posix()}",
                 f"-DCMAKE_RC_COMPILER={(toolchain / 'windres.exe').as_posix()}",
                 f"-DCMAKE_MAKE_PROGRAM={ninja.as_posix()}", "-DBOSHY_BUILD_TESTS=ON",
                 "-DCMAKE_FIND_USE_SYSTEM_ENVIRONMENT_PATH=OFF",
                 "-DCMAKE_FIND_USE_CMAKE_SYSTEM_PATH=OFF",
                 "-DCMAKE_PROGRAM_PATH=" + ";".join(p.replace("\\", "/") for p in program_dirs)]
    if args.offline:
        configure.append("-DFETCHCONTENT_FULLY_DISCONNECTED=ON")
    commands = [configure,
                [str(cmake), "--build", str(build), "--config", args.config,
                 "--parallel", str(args.jobs)],
                [str(cmake.with_name("ctest.exe")), "--test-dir", str(build),
                 "-C", args.config, "--output-on-failure"]]
    for step, command in zip(["configure", "build", "ctest"], commands):
        log = build / f"{step}-{args.config.lower()}.log"
        print(subprocess.list2cmdline(command), flush=True)
        with log.open("wb") as stream:
            result = subprocess.run(command, env=env, cwd=root, stdout=stream, stderr=subprocess.STDOUT)
        print(log.read_text(encoding="utf-8", errors="replace")[-6500:], flush=True)
        if result.returncode:
            print(f"FAILED: {step}, exit {result.returncode}; log: {log}", flush=True)
            return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
