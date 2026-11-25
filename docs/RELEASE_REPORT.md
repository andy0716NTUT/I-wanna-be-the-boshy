# Windows RC1 development report

All required staged technical gates through Phase 6 passed on 2026-11-25
Asia/Taipei, with the measured scope and supplemental manual gaps below.
Branch: remaster/continuous. Original/main remains eeb1f72c6c3a9a9e33a49c31946e4b723323d49f.
Runtime source revision: `0cd2de1fc6aedc8028bc6fa0c584b67f8c6d361f`. Phase 6 changes only build/package tooling and
documentation; `git diff 0cd2de1fc6aedc8028bc6fa0c584b67f8c6d361f -- src include config.json CMakeLists.txt files.cmake`
is empty. This final documentation commit's SHA is read from Git history.
No PR created, main merge, force push or public game publication.

## Internal candidate

- File: `build/release-candidate/BoshyRemaster-Windows-x64-RC1-0cd2de1f.zip` (61,349,149 bytes).
- SHA-256: `2acbb97af2364d0889713ac760a37afcce1c6afb99ed0aedd25605243cc6060d`.
- Adjacent `.sha256` and `-verification.json`; payload includes manifest.json.
- 153 original resources re-counted, four shaders, config, game EXE and exactly
  three recursive non-system PE dependencies: libgcc_s_seh-1.dll,
  libstdc++-6.dll, libwinpthread-1.dll. Compiler and dependency license files copied
  byte-for-byte with provenance. Utilities/probe EXEs are excluded.
- ZIP integrity, extracted payload hashes and normal shipped executable startup
  PASS from C:/Windows with PATH limited to Windows/System32, both directly and
  after extraction under build/可攜驗證. Audio is dummy for every test process.
- Startup smoke runners stop only their own process after 3 seconds; this is not
  a fabricated natural-exit test. Natural shutdown is covered by gameplay/GL
  runners with the same lifetime guard. Missing-Resources negative deployment
  returns nonzero with the precise path, proving no source-tree fallback.
- This is a local/internal validation artifact, not a public game release.

## Stage changes and commits

| Stage | Commit | Result and principal files |
| --- | --- | --- |
| 1 | 8947ffd | Windows Debug/Release/deployment recovery; CMake, RuntimePaths, safe map parsing/tests |
| 1.5 | bf409b3 + db5a07f | Runnable-original generated probes, actual input/events, baseline/resource evidence |
| 2 | f42d7f0 | App-owned PlayerManager runtime; active input/integration/collision extracted with exact original arithmetic |
| 3 | 20e3821 | World bounds, transactional debug teleport, event state ownership and retired renderer owner release |
| 4 | 8a6d96a | Fixed logical framebuffer, nearest/integer bars, resize/fullscreen/mouse mapping, shutdown callback guard |
| 5 | 0cd2de1 | Measured profiles; FPS/state/collision diagnostics, config validation, BGM errors and INFO log default |
| 6 | final Git commit | Fresh build path serialization fix, packaging/portability checks and this report |

Gameplay constants, double gravity, independent directional ordering, X/Z priority,
R state persistence, cached tile queries, map/Checkpoint coordinates, Boss counters
and attack math remain baseline-compatible. The original frame-coupled clock is
retained; no new accumulator, interpolation, difficulty change or asset repaint.
Presentation intentionally becomes nearest/integer and error paths now reject
invalid maps/config/track requests with diagnostics. Shipped sound volume5,
fade/play ordering and same-track restart semantics remain original.

## Actual verification

| Check | Result | Evidence |
| --- | --- | --- |
| Fresh CMake3.31.8/GCC13.1/Ninja Debug and Release | PASS | build/phase6-fresh-*.log, build/windows-rc2 |
| Fresh all CTest | PASS 20/20 each | Debug 2.22s / Release 1.59s; ctest-*.log |
| Full actual-game original/current matrix | PASS 19/19 each config | phase5-gameplay-*.json, source hashes; unchanged runtime in final package |
| Fresh GL native/1080p/1440p/4K/small/fullscreen/restore | PASS 7 modes each config | build/phase6-display-*.log |
| Actual rendered original/current/HUD/player/overlay review | PASS | phase4-scene-review.json, phase5-scene-review.json; generated PNGs in build/visual-* |
| Logical pixel comparisons | PASS | native vs 1080p zero mismatches outside physical Debug panel; 1080p/4K canvases exact |
| All existing maps/masks and malformed/bounds/config paths | PASS | map-loader/world-lookup/runtime-config/runtime-paths CTest |
| Repeated ownership cleanup | PASS | 60/60 checkpoint owners released, 500 headless cycles; no new feasible stress crash |
| Silent BGM decoding/switch/restart/volume | PASS | bgm-silent CTest, asserted dummy device and volume5 |
| Original asset audit | PASS | phase6-asset-audit.json: 153 actual files, zero content drift |
| Packaging/Unicode/minimal-PATH/cwd/negative deployment | PASS | phase6-package.json, payload manifest and ZIP CRC |

The existing cached build initially hid a wrapper defect: native backslashes in
CMAKE_RC_COMPILER generated an invalid escape in a fresh CMakeRCCompiler.cmake.
All compiler/Ninja cache definitions now use forward slashes. The failed
build/windows-rc1 logs were preserved; a new build/windows-rc2 configured and
compiled both configurations cleanly. No game/engine workaround was added.

Exact commands executed (toolchain PATH as in BUILD.md):

```powershell
python B:/scripts/windows_build.py --cmake C:/Users/andyl/AppData/Local/Temp/boshy-tools/cmake-3.31.8-windows-x86_64/bin/cmake.exe --toolchain 'C:/Program Files/JetBrains/CLion 2025.3.3/bin/mingw/bin' --ninja 'C:/Program Files/JetBrains/CLion 2025.3.3/bin/ninja/win/x64/ninja.exe' --build-dir build/windows-rc2 --config Debug --offline
python B:/scripts/windows_build.py --cmake C:/Users/andyl/AppData/Local/Temp/boshy-tools/cmake-3.31.8-windows-x86_64/bin/cmake.exe --toolchain 'C:/Program Files/JetBrains/CLion 2025.3.3/bin/mingw/bin' --ninja 'C:/Program Files/JetBrains/CLion 2025.3.3/bin/ninja/win/x64/ninja.exe' --build-dir build/windows-rc2 --config Release --offline
python B:/scripts/run_gameplay_regression.py --cmake C:/Users/andyl/AppData/Local/Temp/boshy-tools/cmake-3.31.8-windows-x86_64/bin/cmake.exe --config Debug
python B:/scripts/run_gameplay_regression.py --cmake C:/Users/andyl/AppData/Local/Temp/boshy-tools/cmake-3.31.8-windows-x86_64/bin/cmake.exe --config Release
python B:/scripts/audit_baseline.py
python B:/scripts/package_windows_rc.py --build-dir build/windows-rc2 --runtime-licenses 'C:/Program Files/JetBrains/CLion 2025.3.3/bin/mingw/licenses' --objdump 'C:/Program Files/JetBrains/CLion 2025.3.3/bin/mingw/bin/objdump.exe'
```

Also executed CMake3.31.8 `--build B:/build/windows-rc2 --config <Debug|Release>
--target PixelDisplayTests --parallel 8`, then each
`B:/build/windows-rc2/bin/<Config>/PixelDisplayTests.exe` with SDL_AUDIODRIVER=dummy.
Full gameplay matrices were performed in Phase 5 on exactly the final runtime
sources; Phase 6 verifies source identity and fresh normal binaries instead of
claiming that source compilation alone proves gameplay parity.

## Profiling and known limits

Two runs per workload show update p95 about 0.46/0.84/1.60ms after diagnostics for
enemy/reload/Boss defeat, comparable private memory, identical tracked creation
counts, and 94–99% fewer log bytes. Warmup/reload maxima about23ms remain. Live
system noise and limited repeats prevent a broad speedup claim; see PERFORMANCE.md.
Generic heap/GPU/driver allocations are not a complete leak audit.

Eight original death-frame PNGs and initial unused Background/phase1.png remain
absent; original fallback and functional Game Over/R UI are retained. No assets
were invented, renamed, resaved or fetched. The 28 original Windows TXT CRLF
checkout conversions are documented; PNG/audio/font hashes are byte-identical.
The recreation's unfinished/shared World2 layout and existing unusual timer/map
semantics are preserved. Tests cover the declared representative routes/seed,
not every possible world route, random seed or complete original Boshy content.
Human physical-keyboard latency/feel, audible speaker/device playback, other GPU
vendors, DPI/multi-monitor combinations and complete playthrough remain explicit
supplemental checks. Audio-device listening was intentionally not run at the
user's request. No public-distribution rights approval is implied for third-party
original assets.

## Smallest next review

Review the focused Phase 1 build/deployment baseline first, then the independent
stage commits and the internal RC playtest checklist. A human may create a draft
PR after review; no PR was created automatically and main remains unchanged.
