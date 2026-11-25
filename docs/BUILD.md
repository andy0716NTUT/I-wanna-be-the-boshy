# Windows build and recovery

Original reference: `eeb1f72c6c3a9a9e33a49c31946e4b723323d49f`.
No previous Phase 1 commit was found in local history, remote heads/tags or PR refs
on 2026-11-24. This branch re-establishes that baseline on the new computer.

Use C++17, CMake 3.24–3.31 and Ninja. Keep PTSD v0.3-rc1; its archive SHA-256 is
`600da369a106913d16f22ad73d3c9fd4d1c38cfdae05c7ac993f21d67b46360d`.
The pinned upstream dependencies are SDL 2.26.5, SDL_image 2.6.3, SDL_ttf 2.20.2,
SDL_mixer 2.6.3, GLEW 2.2.0, GLM 0.9.9.8, spdlog 1.10.0,
ImGui 1.90.4-docking, nlohmann_json 3.11.3, and GoogleTest 1.13.0.
GoogleTest is already a PTSD dependency; the new project tests use simple executables.

## This computer

- Windows 10.0.26200 x64; PowerShell 7.6.9; Python 3.14.0.
- Existing CLion 2025.3.3 MinGW GCC 13.1.0, binutils/windres 2.40, Ninja 1.12.1.
- No MSYS2/UCRT64 installation found. Reuse this compiler; no system-wide install needed.
- Visual Studio BuildTools 2019 is incomplete, so it is not a verified alternative.
- Portable CMake 3.31.8, verified against the official SHA-256 listing:
  `81aa9964dbabd71fe02e7ec50472fd3ad56138c49944515ece9001efbff8d719`.
  Download: <https://cmake.org/files/v3.31/cmake-3.31.8-windows-x86_64.zip>.
  Checksums: <https://cmake.org/files/v3.31/cmake-3.31.8-SHA-256.txt>.
  Local tool copy: `C:/Users/andyl/AppData/Local/Temp/boshy-tools/cmake-3.31.8-windows-x86_64`.
  A second identical copy and the downloaded archive are ignored under `.tools/`.

Initial original Debug configure succeeded using a Chinese source path. Subsequent
CMake 3.31.8 and bundled 4.1.2 probes exited with `0xC0000409` during automatic
program search, including a minimal independent C++ project. An ASCII alias alone
did not fix it. Explicit compiler/Ninja paths, a bounded `CMAKE_PROGRAM_PATH`, and
disabling system/environment search allowed 3.31.8 configure to complete.
This identifies a working configuration, not a proven Windows/CMake root cause.
The wrapper applies the workaround only to the build process and cache.

## Reproduce

Keep the original source tree in place. `B:` is a reversible ASCII alias to this
repository, created during recovery. Check `subst` and `Test-Path B:/` first; never
replace an existing drive mapping owned by another task. If free:

```powershell
subst B: 'C:\Users\andyl\OneDrive\文件\I-wanna-be-the-boshy'
```

The exact reproducible build commands for this machine are:

```powershell
python B:/scripts/windows_build.py --cmake C:/Users/andyl/AppData/Local/Temp/boshy-tools/cmake-3.31.8-windows-x86_64/bin/cmake.exe --toolchain 'C:/Program Files/JetBrains/CLion 2025.3.3/bin/mingw/bin' --ninja 'C:/Program Files/JetBrains/CLion 2025.3.3/bin/ninja/win/x64/ninja.exe' --config Debug --offline
python B:/scripts/windows_build.py --cmake C:/Users/andyl/AppData/Local/Temp/boshy-tools/cmake-3.31.8-windows-x86_64/bin/cmake.exe --toolchain 'C:/Program Files/JetBrains/CLion 2025.3.3/bin/mingw/bin' --ninja 'C:/Program Files/JetBrains/CLion 2025.3.3/bin/ninja/win/x64/ninja.exe' --config Release --offline
```

Omit `--offline` on a fresh machine so FetchContent can download the pinned sources.
The wrapper prints every exact CMake/build/CTest command and writes logs under
`build/windows/`. `--offline` is valid only after all dependencies are populated.
Do not combine different compilers or source aliases in the same build directory.
For a clean verification use `--build-dir build/windows-clean`, without deleting
the existing evidence. No permanent PATH or security settings are changed.

Equivalent build/test steps after configure:

```powershell
& 'C:/Users/andyl/AppData/Local/Temp/boshy-tools/cmake-3.31.8-windows-x86_64/bin/cmake.exe' --build B:/build/windows --config Debug --parallel 8
& 'C:/Users/andyl/AppData/Local/Temp/boshy-tools/cmake-3.31.8-windows-x86_64/bin/ctest.exe' --test-dir B:/build/windows -C Debug --output-on-failure
& 'C:/Users/andyl/AppData/Local/Temp/boshy-tools/cmake-3.31.8-windows-x86_64/bin/cmake.exe' --build B:/build/windows --config Release --parallel 8
& 'C:/Users/andyl/AppData/Local/Temp/boshy-tools/cmake-3.31.8-windows-x86_64/bin/ctest.exe' --test-dir B:/build/windows -C Release --output-on-failure
```

CTest needs the compiler runtime DLL directory on its process PATH for the test
executables; the wrapper supplies it. Game runtime DLLs are copied beside the game.
`BOSHY_BUILD_TESTS=ON` and `BUILD_TESTING=ON` enable the project and upstream tests.

## Deployment and startup

Each configuration produces `build/windows/bin/<Config>/IWannaBeTheBoshy.exe` with
`Resources/`, `PTSDAssets/shaders/`, unchanged `config.json`, and resolved non-system
DLLs beside it. Copy that entire directory for an internal smoke test. No package
is a Release Candidate until every gate in the master task passes.

Before creating PTSD's context, `main` obtains the executable directory from SDL
and selects it as the process working directory using a Unicode filesystem path.
This is necessary because pinned PTSD opens config and shaders using relative
paths. Startup reports the exact missing deployment path and exits nonzero when
the Resources directory, base shaders or config are missing. Missing individual
images remain upstream diagnostics, including the eight pre-existing death frames.
No source-tree fallback masks an incomplete deployment.

CTest `deployed-resources` compares deployed resource/config/shader bytes with the
source. `runtime-paths` tests directory selection and failure diagnostics without
OpenGL. These do not prove a visible menu or successful PNG/audio decode; record
the actual GUI run separately in the regression checklist.

The original checkout has 153 resources. `scripts/audit_baseline.py` verifies them
against the original Git blobs; 28 TXT files have Git checkout CRLF conversion on
this Windows machine. The original blob and initial checkout SHA-256 values are
both retained in `docs/evidence/original-resources-sha256.json`. No original asset
has been edited or regenerated.

Map safety changes preserve every cell of the 24 original 80x60 maps against the
original parser and all four trap/spider masks against their original CSV parser,
skip blank lines as before, and accept existing comma/space data.
Malformed, empty or missing maps report their path and reason; a failed phase
load clears stale map state. Bounds return `-1` and invalid writes do nothing.
This intentionally changes only error handling for invalid inputs; in-game phase
transition parity must still be verified before further gameplay refactoring.

Optional original/current scene probes and exact frame comparisons are documented
in [REMASTER_BASELINE.md](REMASTER_BASELINE.md). They require graphics and are not
part of the headless CTest pass. They do not complete the entire gameplay gate.

The expanded event/state matrix in [GAMEPLAY_REGRESSION.md](GAMEPLAY_REGRESSION.md)
supersedes the initial partial gate. Enable it with BOSHY_BUILD_REGRESSION_PROBES;
normal builds exclude those executables. All testing is silent (SDL dummy audio
plus zero mixer volumes in probes); ordinary game sound defaults are preserved.
