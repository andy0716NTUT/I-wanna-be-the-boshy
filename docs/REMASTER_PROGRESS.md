# Remaster Progress
TASK_ID: boshy-remaster-continuous
ORIGINAL_SHA: eeb1f72c6c3a9a9e33a49c31946e4b723323d49f
PHASE1_SHA: 8947ffd6b3d78bb34cdefd5d01be11de05fc80f8
WORK_BRANCH: remaster/continuous
HEAD_SHA: db5a07fbf70d4590dd2f7c7505a9059e77e64934
ACTIVE_STAGE: 3
STAGE_STATUS: IN_PROGRESS
NEXT_ACTION: Inspect and detach retired renderer-owned map objects and bullets; bound World lookups, preserve map/event state, then run lifetime stress and full original/current Debug/Release comparisons.
LAST_VERIFIED_AT: 2026-11-25 Asia/Taipei
ENVIRONMENT: Windows 10.0.26200 x64; PowerShell 7.6.9; CLion GCC 13.1.0, binutils 2.40, Ninja 1.12.1; CMake 3.31.8 portable; RTX 5060, OpenGL 4.1 NVIDIA 621.24.

## Phase status table
| Stage | Status | Gate evidence | Commit |
| --- | --- | --- | --- |
| 1 (new Windows verification) | PASSED | Debug/Release compiled, 15/15 CTest each, both menus observed from unrelated cwd; known missing PNGs audited | 8947ffd6b3d78bb34cdefd5d01be11de05fc80f8 |
| 1.5 | PASSED | Debug/Release 15-case actual-game event matrices pass with exact state; original/current death UI visually matches; 15/15 CTest each | This completed baseline commit; resolve via git log |
| 2 | PASSED | 15-case Debug + added death-loop, 16-case Release, 30/120-cap comparisons; 15/15 CTest both | This Phase 2 commit; resolve via git log |
| 3 | IN_PROGRESS | Phase 2 gate passed; lifecycle/world inspection next | None |
| 4 | NOT_STARTED | Requires Stage 3 gate | None |
| 5 | NOT_STARTED | Requires Stage 4 gate | None |
| 6 | NOT_STARTED | Requires all prior gates | None |

## Work completed
- Phase 2 passed: App-owned initialized PlayerManager extracts the active movement/integration/collision path, replacing the incompatible inactive proposal. Wired Debug all 15 original cases + separate new death-loop pass; Release all 16 pass. The final Debug rebuild differs from the full-matrix game only by a newline after the timestep statement; death-loop and 30/120 cap cases use that final build. Raw preflight results with the old wiring are excluded. Preserve double gravity, axis/input priority and R persistence. PLAYER_REFACTOR.md records the deliberately unchanged frame-coupled clock and the new-App state boundary.
- Completed Phase 1.5: both Debug and Release original/current matrices pass all 15 cases (grounded full/short/double jumps, movement/controls, wall/head/one-way, shots/cap/X+Z, spike/death/UI/R, checkpoint/counter-shot/R, room re-entry/traps, enemy, Boss cycle and separate God-mode defeat/victory lifecycle). Normal builds and CTest pass 15/15 each; asset audit has 153 originals and zero content drift. Reports in docs/evidence/gameplay-baseline-{debug,release}.json.
- Graphically inspected original and current death screens through Windows computer-use; matching Game Over/Press R overlay, hidden player and Debug panel. Native Up worked in the original menu; short Return taps were unreliable. The critical gate uses the complete real-update/event equivalent expressly permitted by the master task, not an invented native/human feel pass. No native input implementation changed.
- 2026-11-25 user preference: keep every test game silent. Test runners now use SDL's dummy audio output and probes set music/channel/master volume to zero before App::Start. Interactive visual tests must also set SDL_AUDIODRIVER=dummy in their process environment. Do not change normal-game sound defaults or system volume.
- 2026-11-25 resume: inspected Git/instructions/checkpoint and reproduced Debug/Release CTest (15/15 each, 0.80s / 0.75s). Pushed the two existing commits with `git push -u origin remaster/continuous`; `git ls-remote --heads origin remaster/continuous` confirms bf409b35732b9649d3c017db9e03307b95c80bdd. main unchanged; no PR/merge/public release.
- Phase 1.5 observation work used test-only generated friends and observers; it never changed production behavior. That gate was reopened, completed, committed and pushed as db5a07f. Source manifests identify the copies used for comparison.
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
- Command: `python B:/scripts/run_gameplay_regression.py --cmake C:/Users/andyl/AppData/Local/Temp/boshy-tools/cmake-3.31.8-windows-x86_64/bin/cmake.exe --config Debug`; same with `--config Release`; final Debug `--scenarios death-loop`; Debug `--no-build --scenarios movement full-jump shooting --fps-cap <30|120>`
  Result: PASS
  Evidence: docs/evidence/phase2-gameplay-{debug,release}.json, phase2-death-loop-debug.json, phase2-fps{30,120}.json, phase2-current-sources.json; exact float/state equality, all coverage requirements pass. All processes silent. FPS-cap elapsed values include startup/shutdown; the original changes wall-time speed with cap, so no scheduler change is justified.
- Command: CMake/CTest 3.31.8 absolute executables from BUILD.md, `--build B:/build/windows --config <Debug|Release> --parallel 8`, then `--test-dir B:/build/windows -C <Debug|Release> --output-on-failure`
  Result: PASS
  Evidence: build/phase2-build-{Debug,Release}.log and phase2-ctest-{Debug,Release}.log; 15/15 each (1.14s / 1.06s). First simultaneous deployment failed copying a playing BGM file; all later deployment/tests serialized and clean rerun passes. No code change was needed for that file-sharing failure.
- Command: `python scripts/audit_baseline.py`; `git diff --check`; `python -m py_compile scripts/prepare_regression_sources.py scripts/run_gameplay_regression.py`
  Result: PASS
  Evidence: build/phase2-asset-audit.json; 153 originals, no asset changes. Source drift is intentional refactoring and listed by audit, not falsely called zero.
- Command: `python B:/scripts/run_gameplay_regression.py --cmake C:/Users/andyl/AppData/Local/Temp/boshy-tools/cmake-3.31.8-windows-x86_64/bin/cmake.exe --config Debug`
  Result: PASS
  Evidence: final complete silent Debug run, all 15 cases parity/coverage PASS, including actual received input states. Raw traces: build/gameplay-regression/Debug; summary with hashes: docs/evidence/gameplay-baseline-debug.json.
- Command: `python B:/scripts/run_gameplay_regression.py --cmake C:/Users/andyl/AppData/Local/Temp/boshy-tools/cmake-3.31.8-windows-x86_64/bin/cmake.exe --config Release --no-build`
  Result: PASS
  Evidence: full 15-case Release matrix (normal 60-cap before user mute request); all exact state/coverage comparisons pass, summary docs/evidence/gameplay-baseline-release.json. All subsequent runs are silent; normal-game audio defaults unchanged.
- Command: `cmake --build B:/build/windows --config <Debug|Release> --parallel 8`; `ctest --test-dir B:/build/windows -C <Debug|Release> --output-on-failure`; `python B:/scripts/audit_baseline.py`
  Result: PASS
  Evidence: build/phase15-build-{Debug,Release}.log, build/phase15-ctest-{Debug,Release}.log (15/15 each), build/phase15-asset-audit.json (153 resources, zero gameplay/source/asset drift at baseline). Absolute CMake 3.31.8 command paths remain in BUILD.md.
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
- Eight referenced player death PNGs and initial Background/phase1.png absent in the original. Startup logs the existing missing-texture fallback; menus still render. Death/GameOverUI state and screenshots now verified; the original absent frames remain a known asset limitation.
- Invalid map loads now clear stale map data and reject malformed rows. Valid cells are identical; the measured room re-entry, traps and Boss workloads pass; longer lifecycle stress remains Phase 3 work.
- Compilation and headless tests alone do not establish gameplay parity. Phase 1.5 now has the full declared event/state workloads and visual death-UI checks; coverage scope is explicit in GAMEPLAY_REGRESSION.md.
- Windres RC compilation passes with the B: ASCII alias; full unaliased Chinese-path build remains unverified. Unicode map fixtures pass.
- Automatic short Return taps did not advance Debug menu. Cause not established; do not change original input polling without measured baseline.
- Held SDL events did enter both original/current menus. Native input latency/feel remains unverified; do not infer equivalence from synthetic input alone.
- Historical 320-frame Z samples began while falling and were not grounded jump measurements; the expanded grounded/event matrix supersedes those gaps without changing the earlier raw traces.
- The initial 320-sample gaps are closed by observed velocity/state/events: full jump 120px/34 updates, short 53.000061px/19 updates, shots every 6 updates up to 5; checkpoints, collisions, transitions, enemy and Boss match. These are per-update measurements; native physical feel, untested world routes/devices and audible playback are not certified human passes.
- CMake automatic program search can terminate with 0xC0000409 on this machine; explicit-path workaround verified, root cause not established.

## Decisions and parity exceptions
- Re-established Stage 1 because no reported prior implementation was recoverable. The requested restart from Phase 1 takes precedence over the old assumption that it already passed.
- Use existing GCC 13.1.0 MinGW when no UCRT64 installation exists; keep C++17 and PTSD v0.3-rc1. No engine or gameplay changes authorized by this choice.
- Retained original GNU C++17 extension mode after strict -std=c++17 exposed existing M_PI use. No source constant or arithmetic changed.
- Resource loading error handling intentionally changes invalid-deployment/malformed-map behavior only. Flag for review; no gameplay parity exception approved.
- Scene probes use an alternate test main and common deployment shims; App gameplay sources are original/current, not mocked physics. Record this deviation rather than calling them unmodified original binaries.
- HEAD_SHA names the inspected implementation revision before this documentation/test checkpoint; the checkpoint's own SHA must be read from git log (a file cannot contain its own commit hash). PHASE1_SHA is the exact verified implementation commit.

## Blockers requiring user action
- None for the measured Phase 1.5 gate. Continue the authorized Phase 2 refactor. New unexplained state/physics/timing differences must stop the affected chain; visual/sound/human limitations remain explicit rather than being marked passed by inference.
