# Remaster Progress
TASK_ID: boshy-remaster-continuous
ORIGINAL_SHA: eeb1f72c6c3a9a9e33a49c31946e4b723323d49f
PHASE1_SHA: 8947ffd6b3d78bb34cdefd5d01be11de05fc80f8
WORK_BRANCH: remaster/continuous
HEAD_SHA: 8947ffd6b3d78bb34cdefd5d01be11de05fc80f8
ACTIVE_STAGE: 1.5
STAGE_STATUS: NEEDS_MANUAL_VERIFICATION
NEXT_ACTION: Launch B:/build/windows/bin/Debug/BoshyOriginalGame.exe from its own directory and capture the missing grounded-jump/collision/checkpoint/death/Boss cases in docs/REGRESSION_CHECKLIST.md; compare with IWannaBeTheBoshy.exe before changing physics.
LAST_VERIFIED_AT: 2026-11-24 19:20 Asia/Taipei
ENVIRONMENT: Windows 10.0.26200 x64; PowerShell 7.6.9; CLion GCC 13.1.0, binutils 2.40, Ninja 1.12.1; CMake 3.31.8 portable; RTX 5060, OpenGL 4.1 NVIDIA 621.24.

## Phase status table
| Stage | Status | Gate evidence | Commit |
| --- | --- | --- | --- |
| 1 (new Windows verification) | PASSED | Debug/Release compiled, 15/15 CTest each, both menus observed from unrelated cwd; known missing PNGs audited | 8947ffd6b3d78bb34cdefd5d01be11de05fc80f8 |
| 1.5 | NEEDS_MANUAL_VERIFICATION | Debug and Release each have 320/320 matching original/current scene states; critical event/visual cases still missing | This Phase 1.5 checkpoint commit; resolve via git log |
| 2 | NOT_STARTED | Requires measured gameplay parity | None |
| 3 | NOT_STARTED | Requires Stage 2 gate | None |
| 4 | NOT_STARTED | Requires Stage 3 gate | None |
| 5 | NOT_STARTED | Requires Stage 4 gate | None |
| 6 | NOT_STARTED | Requires all prior gates | None |

## Work completed
- Read the supplied root AGENTS.md and CODEX_MASTER_TASK.md in full. Both were untracked user files on entry; preserve them unchanged.
- Initial branch main, HEAD and origin/main all equal ORIGINAL_SHA. No tracked modifications.
- Live `git ls-remote --heads --tags origin` returns only main at ORIGINAL_SHA, no tags.
- Live PR refs contain only PR #1 at 713d577dd7bd427f9f64d2c7bb4ee0c88150f433. Fetched it read-only; it is the historical merged 2025 bullet change, not Phase 1. GitHub connector confirms this.
- No unreachable Phase 1 commits found by `git fsck --no-reflogs --unreachable`; initial reflog contains only this clone. docs/BUILD.md, docs/REGRESSION_CHECKLIST.md and this progress file did not exist.
- Therefore previous Phase 1 is not available in the inspected repository refs/history. Re-establish Phase 1 from the verified original as explicitly authorized in the current request; do not pretend a previous commit was recovered.
- Created remaster/continuous. No remote write, merge or publication.
- Found existing CLion toolchain outside PATH. No MSYS2/UCRT64 found. Visual Studio 2019 BuildTools is incomplete and not launchable. Reuse MinGW instead of installing another compiler.
- Recovered CMake 3.31.8 with official archive checksum; kept tool copies local/temporary without system installation. B: aliases the existing repository; no source moved.
- Bounded explicit tool search avoids a CMake native crash seen in automatic system search. `scripts/windows_build.py` records exact configure/build/CTest commands and process-local environment.
- Removed the Debug-only application restriction. Resources/config/shaders and non-system runtime DLLs deploy beside each configuration's executable. Runtime startup selects the executable directory before PTSD config initialization and diagnoses incomplete deployment.
- Added map safety checks and lightweight tests: all 24 original 80x60 maps match the original parser cell-for-cell; malformed/missing/empty/overflow/blank-row/bounds/reload/Unicode cases pass.
- Verified 153 original resources, with only 28 pre-existing Git checkout CRLF conversions in text files. Original and checkout hashes retained in a manifest. No asset edit.
- Debug and Release builds each passed all 15 CTest cases, including 12 pre-existing pinned PTSD tests. Actual menus observed in both configurations from C:/Windows with no compiler directory on PATH.
- No physics, collision formulas, controls, original filtering, world geometry or timing code changed. At the Phase 1 menu-only checkpoint, short Return taps did not advance and no gameplay parity had been claimed; subsequent held-key evidence is recorded below.
- Committed Phase 1 as 8947ffd6b3d78bb34cdefd5d01be11de05fc80f8. No push, PR, merge, public release or Release Candidate.
- Added opt-in, excluded graphical scene targets, an immutable original source extractor and a runnable comparison script. No test input or logging code is linked into the normal game.
- Compiled and executed original/current probes in both Debug and Release: 320 frames each, identical logged phase/world/position/tile/input state at every frame, menu entry at frame 21, ten Right updates move 50 pixels. Raw CSV and separate wall-time observations retained.
- Verified comparator rejects a deliberately injected one-pixel position difference at frame 100. A first malformed negative fixture failed before reporting; corrected the fixture to a valid coordinate and reran successfully.
- Built interactive BoshyOriginalGame.exe in Debug and Release from unchanged original main/game sources with build and deployment shims, ready for manual comparison. It has not been manually played.
- Disabled BOSHY_BUILD_SCENE_PROBES again in the local normal build cache; both ordinary builds and full 15-test suites pass. Reference/probe executables remain in the ignored build directory for review.
- Audited real LoadTileData call sites in spider.cpp and added all four original spider/trap masks to the independent original-parser parity tests; tests pass in both configurations.

## Verification evidence
- Command: `git status --short --branch; git branch -avv; git log -12 --oneline --decorate; git remote -v`
  Result: PASS
  Evidence: Original checkout confirmed; only untracked instruction files on entry.
- Command: `git ls-remote --heads --tags origin`
  Result: PASS
  Evidence: Elevated network query returned only main at ORIGINAL_SHA; sandbox query failed with getaddrinfo thread error.
- Command: `git ls-remote origin 'refs/pull/*/head'`
  Result: PASS
  Evidence: Only historical PR #1. Inspected commit and PR metadata.
- Command: `git fsck --no-reflogs --unreachable`
  Result: PASS
  Evidence: No unreachable commits reported.
- Command: `Get-CimInstance Win32_OperatingSystem`
  Result: FAIL
  Evidence: CIM access denied. OS version obtained from PowerShell 7.6.9 instead.
- Command: `python B:/scripts/windows_build.py --cmake C:/Users/andyl/AppData/Local/Temp/boshy-tools/cmake-3.31.8-windows-x86_64/bin/cmake.exe --toolchain 'C:/Program Files/JetBrains/CLion 2025.3.3/bin/mingw/bin' --ninja 'C:/Program Files/JetBrains/CLion 2025.3.3/bin/ninja/win/x64/ninja.exe' --config Debug --offline`
  Result: PASS
  Evidence: build/windows/{configure,build,ctest}-debug.log; 15/15 CTest, 1.69s.
- Command: `python B:/scripts/windows_build.py --cmake C:/Users/andyl/AppData/Local/Temp/boshy-tools/cmake-3.31.8-windows-x86_64/bin/cmake.exe --toolchain 'C:/Program Files/JetBrains/CLion 2025.3.3/bin/mingw/bin' --ninja 'C:/Program Files/JetBrains/CLion 2025.3.3/bin/ninja/win/x64/ninja.exe' --config Release --offline`
  Result: PASS
  Evidence: build/windows/{configure,build,ctest}-release.log; 15/15 CTest, 1.33s.
- Command: `python scripts/audit_baseline.py --write-manifest`
  Result: PASS
  Evidence: docs/evidence/asset-audit-2026-11-24.json; 153 originals, zero content changes, eight missing death PNGs, zero core gameplay source drift.
- Command: `Start-Process -FilePath 'B:/build/windows/bin/<Config>/IWannaBeTheBoshy.exe' -WorkingDirectory 'C:/Windows' ...`
  Result: PASS
  Evidence: actual Debug and Release menus observed via computer-use; config title and OpenGL/shader logs; full command in docs/evidence/phase1-verification.txt. Test processes subsequently closed.
- Command: `git diff --check; python -m py_compile scripts/windows_build.py scripts/audit_baseline.py`
  Result: PASS
  Evidence: no diff whitespace error or Python syntax failure.
- Command: `python B:/scripts/run_scene_comparison.py --cmake C:/Users/andyl/AppData/Local/Temp/boshy-tools/cmake-3.31.8-windows-x86_64/bin/cmake.exe --config Debug`
  Result: PASS
  Evidence: build/baseline/Debug/comparison.json and docs/evidence/scene-comparison.json; both probes exit 0, 320 rows each, no state differences. Explicit tool directories on process PATH as documented in REMASTER_BASELINE.md.
- Command: `python B:/scripts/run_scene_comparison.py --cmake C:/Users/andyl/AppData/Local/Temp/boshy-tools/cmake-3.31.8-windows-x86_64/bin/cmake.exe --config Release`
  Result: PASS
  Evidence: build/baseline/Release/comparison.json and docs/evidence/scene-release-comparison.json; both probes exit 0, 320 rows each, no state differences.
- Command: `cmake --build B:/build/windows --config <Debug|Release> --target BoshyOriginalGame --parallel 8`
  Result: PASS
  Evidence: build/original-game-{Debug,Release}.log; actual CMake absolute path in docs/BUILD.md. Both reference executables built; interactive gameplay not run.
- Command: `python scripts/audit_baseline.py`
  Result: PASS
  Evidence: refreshed docs/evidence/asset-audit-2026-11-24.json, zero core gameplay source drift, original resources intact.
- Command: `python scripts/compare_scene_traces.py build/baseline/Debug/Original.csv build/baseline/negative-drift.csv --report build/baseline/negative-report.json`
  Result: PASS
  Evidence: comparator returns 1 and identifies exactly frame 100 as intended. This is an intentionally failing input, not a real game mismatch.
- Command: `cmake -S B:/ -B B:/build/windows -DBOSHY_BUILD_SCENE_PROBES=OFF`; `cmake --build B:/build/windows --config <Debug|Release> --parallel 8`; `ctest --test-dir B:/build/windows -C <Debug|Release> --output-on-failure`
  Result: PASS
  Evidence: actual tools are the absolute CMake/CTest 3.31.8 paths in docs/BUILD.md; build/normal-configure.log, build/final-build-{Debug,Release}.log, build/final-ctest-{Debug,Release}.log. Final CTest 15/15 each, Debug 0.94s / Release 1.08s. No production src/include/Resources/config diff since PHASE1_SHA.

## Known defects and risks
- Eight referenced player death PNGs and initial Background/phase1.png absent in the original. Startup logs the existing missing-texture fallback; menus still render. Death/GameOverUI flow not yet exercised.
- Invalid map loads now clear stale map data and reject malformed rows. Valid cells are identical; in-game transition/object lifecycle remains untested.
- Compilation and headless tests do not establish movement, collision or boss parity. The partial graphical scene measurement below does not complete the critical gameplay matrix.
- Windres RC compilation passes with the B: ASCII alias; full unaliased Chinese-path build remains unverified. Unicode map fixtures pass.
- Automatic short Return taps did not advance Debug menu. Cause not established; do not change original input polling without measured baseline.
- Held SDL events did enter both original/current menus. Native input latency/feel remains unverified; do not infer equivalence from synthetic input alone.
- The first Z probe occurs while falling; its 110-pixel rise from the pre-input sample is not a grounded jump-height measurement. The second Z has no upward trajectory; jump/death state is not exposed and the cause is not certified.
- No measured velocity/airtime, bullet count/cadence, correct saved checkpoint, ground/head/one-way/spike outcomes, room transition, enemy/Boss or death UI parity. These are critical gate gaps, not implied passes from 320 matched samples.
- CMake automatic program search can terminate with 0xC0000409 on this machine; explicit-path workaround verified, root cause not established.

## Decisions and parity exceptions
- Re-established Stage 1 because no reported prior implementation was recoverable. The requested restart from Phase 1 takes precedence over the old assumption that it already passed.
- Use existing GCC 13.1.0 MinGW when no UCRT64 installation exists; keep C++17 and PTSD v0.3-rc1. No engine or gameplay changes authorized by this choice.
- Retained original GNU C++17 extension mode after strict -std=c++17 exposed existing M_PI use. No source constant or arithmetic changed.
- Resource loading error handling intentionally changes invalid-deployment/malformed-map behavior only. Flag for review; no gameplay parity exception approved.
- Scene probes use an alternate test main and common deployment shims; App gameplay sources are original/current, not mocked physics. Record this deviation rather than calling them unmodified original binaries.
- HEAD_SHA names the inspected implementation revision before this documentation/test checkpoint; the checkpoint's own SHA must be read from git log (a file cannot contain its own commit hash). PHASE1_SHA is the exact verified implementation commit.

## Blockers requiring user action
- Stage 1.5 needs the remaining graphical/native-keyboard cases in docs/REGRESSION_CHECKLIST.md and/or equivalent debug-only event traces. Run the prepared original/current executables sequentially and attach frame/input/coordinate evidence, including clean full/short/second jump, spikes/one-way, checkpoint/R, room transition, enemy and Boss1.
- Existing DebugInfo traces lack velocity, death/ground state, bullet count, checkpoint and Boss events. A safe next PR can add read-only debug instrumentation and focused workloads; do not refactor those systems to make the tests pass.
- Stop gate: CODEX_MASTER_TASK.md says critical untested behavior cannot be PASSED and NEEDS_MANUAL_VERIFICATION is not PASSED. Stage 2–6 remain NOT_STARTED; do not manufacture a Release Candidate.
