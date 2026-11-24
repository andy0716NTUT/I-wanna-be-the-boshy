# Regression checklist

Statuses: `PASS`, `FAIL`, `NOT_RUN`, `MANUAL_REQUIRED`. A passing build or source
comparison is not a gameplay parity result. Update each version column separately.

Original comparison: `eeb1f72c6c3a9a9e33a49c31946e4b723323d49f`.
Windows verification: 2026-11-24, RTX 5060, OpenGL 4.1 NVIDIA 621.24.

| Check | Original runnable | Phase 1 Debug | Phase 1 Release | Evidence / next procedure |
| --- | --- | --- | --- | --- |
| Configure/build | NOT_RUN | PASS | PASS | `scripts/windows_build.py`, `docs/BUILD.md`; original CMake Debug configure only succeeded |
| CTest | NOT_RUN | PASS | PASS | 15/15 per configuration, includes 12 pinned PTSD tests |
| Runtime PNG/config/shader deployment | NOT_RUN | PASS | PASS | Full byte comparison; 153 resource files, config, four shaders |
| Main menu rendered | NOT_RUN | PASS | PASS | Observed actual 1280x960 menu via computer-use; title loaded from config |
| Start from unrelated cwd without compiler PATH | NOT_RUN | PASS | PASS | Start-Process from `C:/Windows`, PATH only System32/Windows; GL and shader logs |
| Initial asset decode | NOT_RUN | PASS | PASS | Menu visible; nine known absent PNG references logged, no replacements |
| Menu navigation / level entry | NOT_RUN | MANUAL_REQUIRED | MANUAL_REQUIRED | Two automated short Return taps did not advance Debug menu; do not call this a pass or change input polling |
| Move left/right, simultaneous keys, up/down | NOT_RUN | NOT_RUN | NOT_RUN | Record fixed input frames and actual displacement on both versions |
| Full jump, early release, second jump | NOT_RUN | NOT_RUN | NOT_RUN | Record apex, frame count, landing coordinates; include X+Z interaction |
| Wall/head/floor/one-way collision | NOT_RUN | NOT_RUN | NOT_RUN | Same room and start position, capture frames before/after each contact |
| Spikes, hidden traps, death UI | NOT_RUN | NOT_RUN | NOT_RUN | Include eight absent death frame references and existing GameOverUI behavior |
| Shoot cadence / five-bullet cap | NOT_RUN | NOT_RUN | NOT_RUN | Count active bullets and shot frames, including holding X and X+Z |
| Checkpoint, delayed activation, R respawn | NOT_RUN | NOT_RUN | NOT_RUN | Record phase, world and checkpoint coordinates before/after death and R |
| Room/world transition and repeat re-entry | NOT_RUN | NOT_RUN | NOT_RUN | Include WORLD1 split phases, WORLD2 rotation and prior-map cleanup |
| Enemy and Boss1 encounter | NOT_RUN | NOT_RUN | NOT_RUN | Same attack sequence and timing; no random-sequence parity claim yet |
| Existing ImGui / cheats | NOT_RUN | NOT_RUN | NOT_RUN | Position/tile displays, gravity controls, teleport and God checkbox |
| BGM/audible playback and switching | NOT_RUN | MANUAL_REQUIRED | MANUAL_REQUIRED | Startup logs have no audio initialization failure; listening/switching not tested |
| High-resolution/fullscreen/pixel filtering | NOT_RUN | NOT_RUN | NOT_RUN | Phase 4 not started; original filtering unchanged |

Map-specific automated coverage:

| Check | Result | Evidence |
| --- | --- | --- |
| All 24 existing 80x60 maps against original parser | PASS | `map-loader`, every cell compared, no asset modification |
| Valid CSV/space data and blank rows | PASS | Fixture through production parser |
| Missing, empty, ragged, invalid integer, overflow | PASS | Reject with path/reason; line number where applicable |
| Bounds reads/writes, SetTile and repeated reload | PASS | `-1` reads, no-op invalid writes, original tile restored |
| UTF-8 Chinese fixture directory | PASS | `測試` fixture, Unicode file open |
| Failed phase load clears previous map | PASS | Valid maps then missing phase, zero dimensions and no stale cell |
| In-game transition / object lifecycle | NOT_RUN | Pure map tests do not exercise App::ReloadMapObjects or Boss state |

For manual capture, build and label both executables with their Git revisions.
Start at native 1280x960 and 60 fps config. Use the same exact input sequence and
starting position; record input frames and wall time separately. Capture menu,
first room, short/full/second jump, wall/one-way/spikes, X held and X+Z,
checkpoint/R, a room transition, bear/bird and Boss1. Save raw traces/screenshots
under ignored `build/regression/`, and commit a small results table with paths and
measured values. `MANUAL_REQUIRED` never advances the Stage 1.5 gate.
