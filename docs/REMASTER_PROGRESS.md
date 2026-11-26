# Remaster Progress
TASK_ID: boshy-remaster-continuous
ORIGINAL_SHA: eeb1f72c6c3a9a9e33a49c31946e4b723323d49f
PHASE1_SHA: 8947ffd6b3d78bb34cdefd5d01be11de05fc80f8
WORK_BRANCH: remaster/continuous
HEAD_SHA: 7095ab71220366d793794a89bd332a6f7b8158b2
ACTIVE_STAGE: 6
STAGE_STATUS: PASSED
ART_BATCH_STATUS: NEEDS_MANUAL_VERIFICATION
CHECKPOINT_REACTION_STATUS: PASSED
NEXT_ACTION: Await human reaction-feel/scenery readability/seam review before further gameplay or default-art changes. Use build/art-preview/Boshy-ArtPreview-v1-RC3-7095ab71/Play-ArtPreview-Silent.cmd; adjacent original-assets launcher uses the same corrected RC3. On resume inspect Git log/status/remote for the documentation-only artifact follow-up after runtime7095ab71. Preserve old RC2/art preview/evidence and originals.
LAST_VERIFIED_AT: 2026-11-26T09:55:20+08:00 Asia/Taipei
ENVIRONMENT: Windows 10.0.26200 x64; PowerShell 7.6.9; CLion GCC 13.1.0, binutils 2.40, Ninja 1.12.1; CMake 3.31.8 portable; RTX 5060, OpenGL 4.1 NVIDIA 621.24.

## Phase status table
| Stage | Status | Gate evidence | Commit |
| --- | --- | --- | --- |
| 1 (new Windows verification) | PASSED | Debug/Release compiled, 15/15 CTest each, both menus observed from unrelated cwd; known missing PNGs audited | 8947ffd6b3d78bb34cdefd5d01be11de05fc80f8 |
| 1.5 | PASSED | Debug/Release 15-case actual-game event matrices pass with exact state; original/current death UI visually matches; 15/15 CTest each | db5a07fbf70d4590dd2f7c7505a9059e77e64934 |
| 2 | PASSED | 15-case Debug + added death-loop, 16-case Release, 30/120-cap comparisons; 15/15 CTest both | f42d7f0735b254690ae604105107886e421d20d2 |
| 3 | PASSED | All 18 original/current cases pass in Debug (two reports) and Release; 17/17 CTest both; 60 real reloads + 500 headless retirement cycles | 20e38216b1e61ef2d2a534afd32786097e4f9ad0 |
| 4 | PASSED | Debug/Release 19-case matrices, 18/18 CTest each, seven real GL sizes/modes each, actual App+ImGui frame review and exact logical pixel comparisons | 8a6d96a (full SHA in git log) |
| 5 | PASSED | Debug/Release all 19 parity cases, 20/20 CTest each, GL matrices, inspected overlay; before/after profiles no major regression, tracked allocations identical | 0cd2de1fc6aedc8028bc6fa0c584b67f8c6d361f |
| 6 | PASSED | Fresh Debug/Release 20/20 CTest, fresh GL modes, final-source19-case parity, 153 assets, packaged/Unicode startup and manifest/negative deployment | This final tooling/report commit; resolve via git log |

## Work completed
- Corrected runtime7095ab71220366d793794a89bd332a6f7b8158b2 committed. Separate internal RC3 ZIP(61361559 bytes) SHA2569f8f6c1051e93cc2d4d8c73982b3e445a8076d7bd74af12008fa090bfd1d15cc passes manifest/extracted manifest/normal Debug+Release+Unicode startup. New scenery preview Boshy-ArtPreview-v1-RC3-7095ab71 has matching corrected EXE,2/2 targeted exact Release replays(movement/checkpoint), themed tuning/reaction30/travel8/sine385/stone/render PASS and31 original PNG backups verified. Old RC2/preview EXEs retain previous hashes. Local artifacts only; report CHECKPOINT_REACTION_FIX.md and checkpoint-reaction-{package,art-preview}.json. Artifact evidence is captured in a documentation-only follow-up; verify remote head on resume.
- Checkpoint interpretation corrected: threshold1.0→0.5 simulation seconds, travel restored16→original8. Debug/Release normal+probe builds PASS;20/20 CTest each(2.10s/1.92s),10/10 unchanged exact game-state scenarios each, native tuning/sine385/stone/R/12reload/render assertions PASS. Actual checkpoint activation4 unchanged, firing65→34(wait61→30 updates), hit70→39, travel8/8, save/R/aim/lifetime PASS. Source153 Resources unchanged. Fresh evidence checkpoint-reaction-*.json and CHECKPOINT_REACTION_FIX.md; old speed16 reports explicitly historical. Commit/package/push next.
- User clarified that checkpoint speed means hit-to-shot reaction, not projectile travel. Reopened this scoped tuning on2026-11-26: implementation/test correction in progress; no new passing claim or artifact yet. Historical0b15f6bf/RC2/art-v1 runtime has the mistaken travel-speed change.
- Scenery v1 committed/pushed as0c8878d7e5d807a3d54f54d89210c6acfe3e1e58 and tagged remaster-art-v1-scenery-20261126. Isolated playable art preview created from unchanged RC2 code0b15f6bf, with31 original PNG backups and separate silent new/original launchers. Actual themed Release replays9/9 match all pre-art states exactly (including Boss3300 updates); themed tipping physics/385 sine frames/R/reload and idle/gone rendered-region comparisons PASS; normal game Unicode/minimal-PATH/unrelated-cwd startup PASS. Source Resources, character/animation assets, old RC2 and C++ remain unchanged. Runtime proof in art/remaster-v1/runtime-verification.json. Visual taste/all rooms/routes remain manual; room7 backdrop held original.
- Scenery batch candidate assembly verified:31 source mappings,27 changed candidate PNGs,3 material tiles,4 byte-exact preserved hazard/collision reference PNGs;119 original PNGs and153 total Resources content unchanged. Exact canvas/alpha and visible spike landmarks pass;5,094,271 protected pixels and2,179,501 material pixels checked; room9 baked/sprite/cover coherence exact. Rejected bad-size/alpha negative controls PASS. Inspected sample scenes and overview; room7 original backdrop deliberately held due registration seams, matched terrain only. CTest20/20 each Debug/Release on existing original-runtime binaries PASS, silent; Temp CMake copy had disappeared and was restored from the existing local3.31.8 copy after a .tools configure0xC0000409. No system install or C++/Resources/config changes.
- User answered category preference: scenery/terrain/scene objects only; characters and animations remain original. Generated/preserved7 new raw art assets (2 backdrops,2 props,3 material textures) plus approved mountain, with prompts. Three trial assemblies retained under ignored build/; image review rejected old-backdrop fragments and room7 seams. Default game/source119 PNGs unchanged. Finalization requires strict texture/template substitution and keeping room7 backdrop as original; do not promote trial images without fresh verification.
- Reversible visual batch explicitly requested: current source1b27388 verified pushed and annotated rollback tag remaster-before-assets-20261126-1b27388 created/pushed. Inventoried119 original PNGs (file/alpha hashes and bounds). Approved mountain reference is now copied into versioned art/remaster-v1/reference for an actual remote image backup; originals and RC2 preserved. Category preference pending; background/scenery work is authorized in either offered scope. No art theme is active in the game.
- User explicitly chose one composition-based redraw preview after the subtle non-AI trial. Built-in image_gen produced a visibly clearer retro pixel mountain scene; preserved as build/asset-preview/world1-mountain-redraw-v1/phasenone-redraw-v1.png (1448x1086,4:3). Prompt, metadata and limits in ASSET_REDRAW_PREVIEW.md / evidence/asset-redraw-world1-v1*. Source153 resources unchanged; no runtime integration, build/gameplay tests or new RC. Overall composition similar, some geometry/plants differ; visual review and correct native-size/layer treatment are needed before any later opt-in deployment.
- One-image non-AI quality trial complete: WORLD1/phasenone.png contrast/color/local-edge treatment exported at original1280x960 with unchanged alpha; original/candidate and static room9 context sheets reviewed. Final local directory build/asset-preview/world1-mountain-v1-final; report ASSET_QUALITY_PREVIEW.md and evidence/asset-preview-world1-v1.json. All153 original assets unchanged; no runtime deployment, build or gameplay test in this preview task. Strong original blur remains; manual taste review precedes any later opt-in rollout. RC2 remains at0b15f6bf.
- Post-RC user tuning complete and pushed as 0b15f6bf1e8a7ec116e3a5208df39e8c3f4aa85e. RC2 ZIP BoshyRemaster-Windows-x64-RC2-0b15f6bf.zip (61361588 bytes), SHA256 70f1d026824452dee934a33e7858013e8d1494495083b6b1b1d51871d6ec8c78. Package manifest and normal Debug/Release/Unicode-extracted startup from C:/Windows/minimal PATH all PASS, silent. Evidence gameplay-tuning-package.json; RC1 preserved. Remaining work is optional human feel/corner playtest only; main unchanged, no public binary release or PR.
- Requested tuning now passes both Debug/Release18/18 strict unchanged scenarios,20/20 CTest each, native new-scene/counter-shot assertions including jumping off the slope, and inspected native/1080p/4K frames. Source153 assets unchanged. Small evidence reports are archived as docs/evidence/gameplay-tuning-*.json; original strict parity runner remains intact. Awaiting focused commit/push and new internal RC2 packaging only.
- Post-RC tuning verification: Debug18/18 unchanged gameplay cases PASS; Debug/Release20 CTest each PASS; native tuning/actual counter-shot checks PASS both, Release1080p/4K original-region pixels PASS. Real stone landing begins tip at frame7, free fall26, gone62; 19 supported tip frames; R +12 reloads + re-entry restore original cells. Shot firing frame65 unchanged, speed8→16, hit70→67. Release18-case parity still running. No final tuning acceptance or package yet.
- User-authorized post-RC tuning in progress: WORLD1 room2 keeps alternating emergence (latest correction overrides earlier mirrored-choice reply), shares one sine clock with original 480/190 strokes; room9 right stone gets loaded-edge tipping then accelerated fall; global checkpoint counter-shot travel 8→16 per update only. Original assets remain byte-identical. Initial Debug build/20 CTest pass; new real-scene probe and final verification pending.
- Final remote audit: main remains eeb1f72; remaster/continuous is at the accepted runtime commit before this final tooling/report checkpoint; only historical PR #1 ref exists. RC archive checksum reverified after report generation. Preserve the two untracked user instruction files.
- Phase 6 passed: fresh windows-rc2 Debug/Release each20 CTest (2.22s/1.59s); fresh actual GL modes all pass. Runtime source identity matches final Phase519-case matrices. Packaged actual normal EXE starts from C:/Windows/minimal PATH in Debug, Release and ZIP-extracted Unicode directory; missing-Resources fails nonzero with exact path. All tests silent. Original asset audit re-counts153 with zero content drift.
- Internal RC1 created with manifest, recursive required DLLs, supplied license texts and SHA256. Local artifact details/checksum in RELEASE_REPORT.md and phase6-package.json. No binary upload/public release/main merge/PR. All gates complete within declared measured scope; physical keyboard/audio and other hardware/routes remain supplemental manual limits.
- Phase 6 fresh configure exposed wrapper backslash paths in generated CMakeRCCompiler.cmake (invalid escape \P), masked by the existing cache. Normalize CMake compiler/Ninja definitions to forward slashes. Preserve failed build/windows-rc1 logs; retry genuinely fresh build/windows-rc2. Runtime/gameplay source unchanged since Phase 5.
- Phase 5 passed: Debug/Release 19/19 exact gameplay state, normal builds and 20/20 CTest (1.56s/1.38s), display matrices and inspected actual overlay/UI frames. Profiling shows comparable private memory and unchanged tracked allocations, INFO reduces trace bytes 94–99%; no speculative resource rewrite. Invalid config/BGM errors are deliberate error-path changes only.
- Phase 5 implemented read-only FPS/frame-time/velocity/state/object counts, optional clipped platform-contact/cached-tile overlay and wider Debug panel; same controls remain. Added early typed config validation (path/reason), INFO shipped log level and BGM path/load/status diagnostics. Valid BGM volume5, fade/play order and same-track R restart preserved; invalid destinations now keep current track with error. Release normal build + 20/20 CTest pass, including dummy-device BGM decoding/restart and invalid configuration checks.
- Baseline/after Release profiles (2 repeats of enemy/60-reload/Boss-defeat): tracked allocations identical, p95 App updates ~0.46/0.84/1.60ms after vs ~0.51/1.18/1.57ms before. Peak private memory comparable (235.95/276.98/238.29 MiB after); log sizes reduced 94-99%. No speculative pooling/cache or Boss platform deduplication. Full parity/Debug/overlay checks pending.
- Phase 4 passed on resume: Release all 19 cases exact, completing Debug 19/19 + both 18/18 CTest + real GL matrices and inspected game backbuffers. Source PNG/audio unchanged. Scene-only nearest/integer presentation and shutdown-only GL callback guard accepted for this measured scope. No human keyboard/audio-device certification implied.
- Resumed Phase 4: Debug/Release final build and 18/18 CTest pass (0.77s/1.40s). GL target tests pass all seven actual sizes/fullscreen/restores, including every fractional-stripe output sample. Corrected only a test's equal-distance nearest-neighbor tie assumption after logging the exact failing pixel; no rendering implementation or physics expectation changed.
- Replaced interrupted desktop capture with test-only actual App+ImGui backbuffer export. Inspected original/current native, current 1080p, 4K and player outputs; 1080p/4K logical canvases are exact, native vs 1080p is exact outside known physical Debug panel. Graphical review now PASS by this safe automatic equivalent; report phase4-scene-review.json. Release full 19-case matrix still running.
- 2026-11-25 resume authorized by user: verified branch/HEAD and preserved the uncommitted Phase 4 candidate. Resume pending Release/CTest/graphical gate with silent audio. Strengthen pixel readback to cover fractional nearest sampling as well as integer scaling.
- Interruption checkpoint: user stopped Computer Use with the physical Escape key. No further computer input issued; only the specifically recorded visual test PID was safely closed and this checkpoint saved. The first 45-second visual attempt expired and did not capture the game; it does NOT count as scene review. The longer test was interrupted before a valid game screenshot. Do not include unrelated desktop content in repository evidence.
- Phase 4 actual GL tests now PASS in Debug and Release at all seven requested/restore configurations; full Debug gameplay has 19/19 matching cases across phase4-gameplay-debug-{display,core}.json. Release targets built, but full Release gameplay and final all-18 CTest have NOT run. No Phase 4 acceptance/commit, Phase 5 or Phase 6 claim is made. Numerical matrices use no visual-hold argument; changing the optional pause from 45 to 180 seconds affects visual-only runs.
- Phase 4 in progress: fixed 1280x960 color/depth canvas, scene-only nearest sampler and integer centered viewport; physical ImGui and explicit DPI-aware mouse mapping, resize and fullscreen. PTSD/source assets unchanged. Debug GL stripe/black-bar/sample tests PASS at actual native, 1080p, 1440p, 4K, smaller window, desktop fullscreen and restored window. Pure viewport test passes. Full game/Release/GUI gates pending.
- GDB identified a repeatable 0xC0000005 after main during Core::Context global destruction: driver performance callback called destroyed spdlog logger. Added GlDebugLifetime to normal/test entry points to unregister only at scope exit, preserving runtime GL diagnostics. Debug display test now exits 0. Logs: build/phase4-display-gdb-full.log (private build evidence), phase4-display-debug.log. Original gameplay logic and engine sources unchanged.
- Phase 3 implementation: bounded World lookup, preserving TryLoadMap for rejected debug teleport, explicit App-owned event timers with original phase-8 re-entry quirk, renderer detachment at original object retirement points. Debug stress proves 60/60 old Checkpoints released (Original 0/60); 500 headless lifetime cycles and map/world tests pass. All 18 cases now pass in Debug and Release, and CTest passes 17/17 each; see WORLD_LIFECYCLE.md.
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
- Command: `python B:/scripts/windows_build.py --cmake C:/Users/andyl/AppData/Local/Temp/boshy-tools/cmake-3.31.8-windows-x86_64/bin/cmake.exe --toolchain 'C:/Program Files/JetBrains/CLion 2025.3.3/bin/mingw/bin' --ninja 'C:/Program Files/JetBrains/CLion 2025.3.3/bin/ninja/win/x64/ninja.exe' --build-dir build/windows-rc2 --config <Debug|Release> --offline`
  Result: PASS
  Evidence: fresh config/build/ctest logs under build/windows-rc2;20/20 each,2.22s/1.59s. Initial rc1 fresh configure FAIL due backslash windres path; normalized cache definitions, then new rc2 builds pass. No runtime changes.
- Command: CMake3.31.8 `--build B:/build/windows-rc2 --config <Debug|Release> --target PixelDisplayTests --parallel 8`, then corresponding PixelDisplayTests.exe with dummy audio; `python B:/scripts/audit_baseline.py`
  Result: PASS
  Evidence: phase6-display-*.log all7 actual sizes/modes per config; phase6-asset-audit.json153 files/no content drift. Final19-case gameplay reports remain valid because src/include/config/CMake are identical to the Phase5 runtime revision.
- Command: `python B:/scripts/package_windows_rc.py --build-dir build/windows-rc2 --runtime-licenses 'C:/Program Files/JetBrains/CLion 2025.3.3/bin/mingw/licenses' --objdump 'C:/Program Files/JetBrains/CLion 2025.3.3/bin/mingw/bin/objdump.exe'`
  Result: PASS
  Evidence: phase6-package.json; packaged/Unicode/minimal PATH/cwd startup, ZIP integrity and extracted239-file manifest pass. Three required runtime DLLs only. Additional missing-resource actual-exe negative test exits1/path diagnostic. Startup smoke processes explicitly terminated by their owning runner; natural exit not inferred from them.
- Command: `python B:/scripts/profile_game.py --label before --repeats 2`; same with `--label after`
  Result: PASS
  Evidence: phase5-profile-{before,after}.json and PERFORMANCE.md, 6 runs per group at 60 cap/dummy audio. p95 <1.7ms, comparable peak private memory, identical selected game-side image/animation/actor creation counts. Generic heap/GPU allocations not claimed as fully instrumented.
- Command: `python B:/scripts/run_gameplay_regression.py --cmake C:/Users/andyl/AppData/Local/Temp/boshy-tools/cmake-3.31.8-windows-x86_64/bin/cmake.exe --config <Debug|Release>`
  Result: PASS
  Evidence: phase5-gameplay-{debug,release}.json, all 19 cases each; phase5-current-sources.json. Same exact-state comparator, no widened physics tolerance or expected gameplay changes.
- Command: CMake3.31.8 `--build B:/build/windows --config <Debug|Release> --target all PixelDisplayTests --parallel 8`; CTest3.31.8 `--test-dir B:/build/windows -C <Debug|Release> --output-on-failure`; `B:/build/windows/bin/<Config>/PixelDisplayTests.exe`; `python B:/scripts/capture_game_frames.py --output-dir build/visual-phase5 --overlay`
  Result: PASS
  Evidence: phase5-final-build/ctest/display-{Debug,Release}.log, 20/20 each; phase5-scene-review.json. All BGM tests explicitly assert dummy driver, decoded tracks, active playback and original volume5; physical listening intentionally not run. BGM source initially missed direct Logger include, fixed and rebuilt successfully.
- Command: CMake 3.31.8 `--build B:/build/windows --config <Debug|Release> --target PixelDisplayTests DisplayViewportTests --parallel 8`; `B:/build/windows/<Config>/DisplayViewportTests.exe`; `B:/build/windows/bin/<Config>/PixelDisplayTests.exe` with SDL_AUDIODRIVER=dummy
  Result: PASS
  Evidence: build/phase4-display-{debug,release}.log, docs/evidence/phase4-display.json; actual native/1080p/1440p/4K/small/fullscreen/restore sizes, nearest sampler, stripe pixels, bars, mouse center and default framebuffer binding; exit 0 after shutdown guard. Initial missing generated source was fixed by prepare_regression_sources.py; a missing test iostream header was fixed; GDB identified and verified the logger teardown fix. Real-game graphical scene review remains NOT_COMPLETED, not inferred from GL assertions.
- Command: `python B:/scripts/run_gameplay_regression.py --cmake C:/Users/andyl/AppData/Local/Temp/boshy-tools/cmake-3.31.8-windows-x86_64/bin/cmake.exe --config Debug --scenarios resize-play full-jump movement shooting checkpoint transition spikes death-loop enemy boss boss-death spider-bird reload-stress`; Debug `--no-build --scenarios short-jump double-jump wall head oneway shoot-jump`
  Result: PASS
  Evidence: phase4-gameplay-debug-{display,core}.json, 13+6 cases. Release full matrix and all 18 CTest are NOT_RUN for this candidate. Release game/probe/display build succeeds in build/phase4-release-build.log.
- Command: `python scripts/audit_baseline.py`
  Result: PASS
  Evidence: build/phase4-asset-audit.json; actual count 153, changed_resources empty. No image/audio/shader source edits.
- Command: `python B:/scripts/run_gameplay_regression.py --cmake C:/Users/andyl/AppData/Local/Temp/boshy-tools/cmake-3.31.8-windows-x86_64/bin/cmake.exe --config Debug --scenarios reload-stress spider-bird transition checkpoint enemy boss boss-death`; Debug `--no-build --scenarios full-jump short-jump double-jump movement wall head oneway shooting shoot-jump spikes death-loop`; Release default full matrix
  Result: PASS
  Evidence: docs/evidence/phase3-gameplay-debug-{world,core}.json covers all 18; phase3-gameplay-release.json all 18. Exact fields include spider/bird pose/state/timers, tile hashes, received input and projectiles. source hashes in phase3-current-sources.json. All runs silent.
- Command: `cmake --build B:/build/windows --config <Debug|Release> --parallel 8`; `ctest --test-dir B:/build/windows -C <Debug|Release> --output-on-failure` (absolute CMake/CTest 3.31.8 from BUILD.md)
  Result: PASS
  Evidence: build/phase3-build-{Debug,Release}.log; phase3-ctest-{Debug,Release}.log, 17/17 each (1.84s / 1.59s). The new console lifetime test initially linked as SDL_main; SDL_MAIN_HANDLED corrected its entry point and the rerun passes. No game entry-point change.
- Command: `ctest --test-dir B:/build/windows -C Debug -R 'map-loader|world-lookup|scene-lifetime' --output-on-failure`
  Result: PASS
  Evidence: focused 3/3; phase3-lifetime.json contains actual 60 reload weak-owner counts (Original 0 released, Current 60) and log hashes, plus 500 headless cleanup cycles. Invalid debug destinations preserve room; malformed/absent/transactional maps and ragged-world bounds pass. Valid source assets and coordinate math unchanged.
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
- User explicitly requested these post-RC exceptions: WORLD1/2 keeps alternating emergence with a common sine clock and original strokes; WORLD1/9 right stone tips then falls with scoped support/cleared tiles; global checkpoint travel8→16. Latest correction overrides the earlier mirrored-opening reply. Full measured results are in GAMEPLAY_TUNING.md.
- Re-established Stage 1 because no reported prior implementation was recoverable. The requested restart from Phase 1 takes precedence over the old assumption that it already passed.
- Use existing GCC 13.1.0 MinGW when no UCRT64 installation exists; keep C++17 and PTSD v0.3-rc1. No engine or gameplay changes authorized by this choice.
- Retained original GNU C++17 extension mode after strict -std=c++17 exposed existing M_PI use. No source constant or arithmetic changed.
- Resource loading error handling intentionally changes invalid-deployment/malformed-map behavior only. Flag for review; no gameplay parity exception approved.
- Scene probes use an alternate test main and common deployment shims; App gameplay sources are original/current, not mocked physics. Record this deviation rather than calling them unmodified original binaries.
- HEAD_SHA names the inspected implementation revision before this documentation/test checkpoint; the checkpoint's own SHA must be read from git log (a file cannot contain its own commit hash). PHASE1_SHA is the exact verified implementation commit.

## Blockers requiring user action
- None for the completed internal technical RC gates. Supplemental physical input/audio/device/full-route testing is explicitly unclaimed. Public release and main merge remain outside the authorized scope.
