# Remaster Progress
TASK_ID: boshy-remaster-continuous
ORIGINAL_SHA: eeb1f72c6c3a9a9e33a49c31946e4b723323d49f
PHASE1_SHA: UNKNOWN
WORK_BRANCH: remaster/continuous
HEAD_SHA: eeb1f72c6c3a9a9e33a49c31946e4b723323d49f
ACTIVE_STAGE: 1.5
STAGE_STATUS: IN_PROGRESS
NEXT_ACTION: Commit the verified Windows Phase 1 restoration, then establish original-versus-Phase-1 gameplay evidence without changing physics.
LAST_VERIFIED_AT: 2026-11-24 19:10 Asia/Taipei
ENVIRONMENT: Windows 10.0.26200 x64; PowerShell 7.6.9; CLion GCC 13.1.0, binutils 2.40, Ninja 1.12.1; CMake 3.31.8 portable; RTX 5060, OpenGL 4.1 NVIDIA 621.24.

## Phase status table
| Stage | Status | Gate evidence | Commit |
| --- | --- | --- | --- |
| 1 (new Windows verification) | PASSED | Debug/Release compiled, 15/15 CTest each, both menus observed from unrelated cwd; known missing PNGs audited | This pending restoration commit |
| 1.5 | IN_PROGRESS | Static assets/map parity established; gameplay measurements missing | None |
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
- No physics, collision formulas, controls, original filtering, world geometry or timing code changed. Automatic menu navigation did not succeed with short Return taps; no gameplay parity claimed.

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

## Known defects and risks
- Eight referenced player death PNGs and initial Background/phase1.png absent in the original. Startup logs the existing missing-texture fallback; menus still render. Death/GameOverUI flow not yet exercised.
- Invalid map loads now clear stale map data and reject malformed rows. Valid cells are identical; in-game transition/object lifecycle remains untested.
- Compilation and headless tests do not establish movement, collision or boss parity. No critical gameplay measurements have run yet.
- Windres RC compilation passes with the B: ASCII alias; full unaliased Chinese-path build remains unverified. Unicode map fixtures pass.
- Automatic short Return taps did not advance Debug menu. Cause not established; do not change original input polling without measured baseline.
- CMake automatic program search can terminate with 0xC0000409 on this machine; explicit-path workaround verified, root cause not established.

## Decisions and parity exceptions
- Re-established Stage 1 because no reported prior implementation was recoverable. The requested restart from Phase 1 takes precedence over the old assumption that it already passed.
- Use existing GCC 13.1.0 MinGW when no UCRT64 installation exists; keep C++17 and PTSD v0.3-rc1. No engine or gameplay changes authorized by this choice.
- Retained original GNU C++17 extension mode after strict -std=c++17 exposed existing M_PI use. No source constant or arithmetic changed.
- Resource loading error handling intentionally changes invalid-deployment/malformed-map behavior only. Flag for review; no gameplay parity exception approved.

## Blockers requiring user action
- None for the completed Phase 1. Stage 1.5 may require manual held-key gameplay capture if a safe frame-scripted substitute cannot establish equivalence.
