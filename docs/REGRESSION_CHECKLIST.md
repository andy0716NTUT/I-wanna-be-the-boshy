# Regression checklist

Statuses: `PASS`, `FAIL`, `NOT_RUN`, `MANUAL_REQUIRED`. A passing build or source
comparison is not a gameplay parity result. Update each version column separately.

Original comparison: `eeb1f72c6c3a9a9e33a49c31946e4b723323d49f`.
Windows verification: 2026-11-24, RTX 5060, OpenGL 4.1 NVIDIA 621.24.

| Check | Original runnable | Phase 1 Debug | Phase 1 Release | Evidence / next procedure |
| --- | --- | --- | --- | --- |
| Configure/build | PASS (build shims) | PASS | PASS | Original game and probe targets built in Debug/Release using recovered CMake and relative asset definitions; original CMake itself Debug configured only |
| CTest | NOT_RUN | PASS | PASS | 15/15 per configuration, includes 12 pinned PTSD tests |
| Runtime PNG/config/shader deployment | NOT_RUN | PASS | PASS | Full byte comparison; 153 resource files, config, four shaders |
| Main menu rendered | NOT_RUN | PASS | PASS | Observed actual 1280x960 menu via computer-use; title loaded from config |
| Start from unrelated cwd without compiler PATH | NOT_RUN | PASS | PASS | Start-Process from `C:/Windows`, PATH only System32/Windows; GL and shader logs |
| Initial asset decode | NOT_RUN | PASS | PASS | Menu visible; nine known absent PNG references logged, no replacements |
| Scripted menu navigation / level entry | PASS | PASS | PASS | Both original/current 320-frame probes enter WORLD1 at frame 21 in Debug and Release |
| Native-keyboard menu / input feel | MANUAL_REQUIRED | MANUAL_REQUIRED | MANUAL_REQUIRED | Two short GUI Return taps did not advance Debug; held SDL probe events do; no polling change justified |
| Move left/right, simultaneous keys, up/down | PASS | PASS | PASS | Full state/input matrix; Right +50, Left -50, simultaneous cancellation; original vertical controls retained |
| Full jump, early release, second jump | PASS | PASS | PASS | Grounded fixtures; 120px / 34 updates, short-hop 53.000061px / 19 updates, second-jump cap 2 |
| Wall/head/floor/one-way collision | PASS | PASS | PASS | Actual tile-2 head/wall and tile-8 pass-through/landing events matched |
| Spikes, moving traps, death UI | PASS | PASS | PASS | Actual tile-5 death, matched trap tile hashes/UI state and R; original/current death screens visually inspected |
| Shoot cadence / five-bullet cap | PASS | PASS | PASS | Six-update cadence, cap 5, X+Z precedence; actual projectiles and lifetimes compared |
| Checkpoint, save and R respawn | PASS | PASS | PASS | Real bullet activation, save/counter-shot and recorded checkpoint restored by R |
| Room transition and repeat re-entry | PASS | PASS | PASS | Room 1→2→1, trap lifecycle and tile hashes; additional world routes remain outside this workload |
| Enemy and Boss1 encounter | PASS | PASS | PASS | Bear, all five Boss attack states with seeded RNG, projectile damage; separate God-mode defeat/platform-lifecycle case |
| Existing ImGui / debug paths | PASS | PASS | PASS | Actual panel rendered in original/current death scenes; existing teleport/God paths exercised by declared fixture setup |
| BGM/audible playback and switching | NOT_RUN | MANUAL_REQUIRED | MANUAL_REQUIRED | Startup logs have no audio initialization failure; listening/switching not tested |
| High-resolution/fullscreen/pixel filtering | NOT_RUN | NOT_RUN | NOT_RUN | Phase 4 not started; original filtering unchanged |

Map-specific automated coverage:

| Check | Result | Evidence |
| --- | --- | --- |
| All 24 existing 80x60 maps against original parser | PASS | `map-loader`, every cell compared, no asset modification |
| Four original trap/spider masks against original CSV parser | PASS | `map-loader`, every cell compared, including original trailing commas |
| Valid CSV/space data and blank rows | PASS | Fixture through production parser |
| Missing, empty, ragged, invalid integer, overflow | PASS | Reject with path/reason; line number where applicable |
| Bounds reads/writes, SetTile and repeated reload | PASS | `-1` reads, no-op invalid writes, original tile restored |
| UTF-8 Chinese fixture directory | PASS | `測試` fixture, Unicode file open |
| Failed phase load clears previous map | PASS | Valid maps then missing phase, zero dimensions and no stale cell |
| In-game transition / object lifecycle workload | PASS | Actual App reload, room re-entry, enemy and Boss defeat/platform matrix; leak/performance profiling belongs to later gates |

For manual capture, build and label both executables with their Git revisions.
Start at native 1280x960 and 60 fps config. Use the same exact input sequence and
starting position; record input frames and wall time separately. Capture menu,
first room, short/full/second jump, wall/one-way/spikes, X held and X+Z,
checkpoint/R, a room transition, bear/bird and Boss1. Save raw traces/screenshots
under ignored `build/regression/`, and commit a small results table with paths and
measured values. `MANUAL_REQUIRED` never advances the Stage 1.5 gate.

Scripted partial evidence is in [REMASTER_BASELINE.md](REMASTER_BASELINE.md) and
the checked-in raw scene traces. Debug and Release each matched all 320 logged
states; this does not change the untested rows above to PASS.

2026-11-25 update: the expanded 15-case original/current event matrix independently
closed the critical numerical/scene gaps; see [GAMEPLAY_REGRESSION.md](GAMEPLAY_REGRESSION.md).
The original/current source and raw-data hashes are recorded in checked-in reports.
Native physical keyboard feel, audible output and other world routes remain
explicit supplemental coverage limits, not fabricated human passes. Test audio
is forced silent per the user's request. Automated equivalence is the gate basis.

Phase 2: original/current wired player refactor passes Debug all previous 15 cases
plus a separate three-death-loop run, Release all 16, and focused Debug movement/
jump/shoot at caps 30 and 120. CTest 15/15 each. No physics/timing/collision
tolerance or expected-value adjustment. Reports: docs/evidence/phase2-*.json.
