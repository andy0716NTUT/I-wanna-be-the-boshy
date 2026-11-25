# Performance and diagnostics

Machine: Windows 10.0.26200, RTX 5060 / NVIDIA 621.24, GCC 13.1.0, CMake 3.31.8,
Release, normal 60-cap, dummy audio. `profile_game.py --label before|after --repeats 2`
runs owned real-game probes from C:/Windows and samples private process memory/CPU
with existing psutil 7.1.3 every 50ms. No desktop capture or input is used.

Test-only ProbeProfile counts selected game-side make_shared sites (images,
animations, actors). It excludes engine-internal frame images, generic heap,
GPU/driver and C allocations: this is a representative creation baseline, not
complete allocator/leak certification. Counters are disabled for normal parity
runs and absent from the shipped game. Timing includes actual App::Update+render;
fixture time is separate, frame time includes fixture/input/swap/cap, excludes
trace serialization. Startup is Context+App::Start, not whole process launch.
The profiler itself/trace IO adds overhead identically to both measurements.

| Workload | Update p95 before/after ms | Peak private before/after MiB | Log bytes before/after | Tracked images/animations/actors |
| --- | --- | --- | --- | --- |
| enemy | 0.507 / 0.456 | 235.27 / 235.95 | 180230 / 1857 | 1/2/1 |
| reload-stress | 1.178 / 0.844 | 278.79 / 276.98 | 403671 / 24051 | 180/90/270 |
| boss-death | 1.571 / 1.600 | 237.21 / 238.29 | 440500 / 2425 | 352/74/424 |

Values average two runs; raw maxima/CPU/load-time data and binary/config hashes
are in docs/evidence/phase5-profile-{before,after}.json. Small differences are
inconclusive on a live desktop. All p95 updates remain far below the 16.67ms
60-Hz frame budget; isolated warmup/reload spikes reach about 23ms in both groups.
Do not claim smoother physics or a statistically proven speedup. Deterministic
log volume drops 94–99% after INFO replaces per-frame TRACE in shipped config.
ERROR/WARN diagnostics remain. Set default_log_level=0 explicitly for old tracing.

Resource review: Phase 3 detached retired renderer owners; 60 actual checkpoint
reloads release 60/60 and 500 headless cleanup cycles pass. Current tracked
creation counts remain identical after diagnostics. Boss victory deliberately
rescans/recreates platforms; no pooling/deduplication/cache rewrite is attempted
because timing/overlap behavior would need a separate comparison. Existing PTSD
surface/music stores cache bounded observed paths; aggregate private memory is
comparable here. These measurements do not certify every engine/driver allocation.

## Debug UI and settings

Debug now has FPS/frame time, per-update vertical velocity/jump count, death state,
active counts, BGM path/load time/error and optional collision visualization. Green
is the original 16x16 platform-contact body; yellow marks the five cached tile
queries before integration. These are not a replacement for distinct trap/Boss
radius tests. Overlay clips to content and uses DPI-aware world-to-window mapping;
its default is off. A minimum 390px panel width fixes clipped diagnostics while
preserving old controls and scrolling. No input binding or physics timing changes.

Runtime configuration validates JSON/object/types, title, fullscreen, log level
0–6, unsigned integral FPS cap (0 keeps original uncapped mode), and physical
window width320–8192/height240–8192 before creating the context. Unknown keys are
retained/ignored as before; errors name the config path. Bounds protect SDL window
creation from invalid/oversized values. Source config still uses 1280x960/60fps.

BGM diagnostics validate paths, retain current playback for a missing destination,
and report play failure/load time. Valid-track volume5, constructor Play→FadeIn(2),
SetBGM Pause→Load→FadeIn(2)→Play, and repeated same-track R restarts remain in their
original order. The existing fade semantics are not silently fixed. Silent BGM
CTest loads MENU/WORLD1/WORLD2/Boss1, repeats WORLD1 and checks mixer activity and
volume5 on the dummy device. No audible device/listening certification is claimed.

Final gate: Debug/Release all 19 original-state comparisons and 20/20 CTest pass.
Actual exported native/1080p player frames were inspected with overlay enabled;
fields fit the wider panel, boxes align and clip correctly. Existing controls
remain accessible by scrolling. Valid gameplay/timing unchanged in measured cases.
