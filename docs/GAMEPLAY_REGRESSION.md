# Executed gameplay regression protocol

Reference: original `eeb1f72c6c3a9a9e33a49c31946e4b723323d49f`; measured Phase 1
implementation `8947ffd6b3d78bb34cdefd5d01be11de05fc80f8`. The normal gameplay
sources remained unchanged through the Phase 1.5 instrumented baseline capture.

The earlier 320-row DebugInfo probe was insufficient for the physics gate. The
new opt-in targets compile test-only copies of the actual game sources. Generated
copies add friend declarations and scope/event observers, with original and
instrumented source SHA-256 manifests. No gameplay statements are replaced or
mocked; no friends, input script or observer is linked into the normal game.
The reference target uses a frozen list of translation units actually compiled
by the original CMake, excluding its unused, incompatible PlayerManager proposal.

Each scenario starts in a fresh process, enters through the real menu with held
SDL Return events, and uses existing debug teleport plus declared player-position
fixtures in unmodified maps. Test placement is not a claim of a native-keyboard
playthrough. Inputs pass through the pinned SDL/PTSD input path and App::Update;
RenderImGui and the real renderer run. RNG is seeded identically in test processes.

Snapshots record post-update position, velocity, jump count, cooldown, accepted
events, player/death-UI visibility, UI timer/scale, world/room, checkpoint, bullet
positions/lifetimes, tile-data hash, enemy position, Boss state/health/attacks and
projectiles. Floats use max_digits10. Matching requires exact logged state;
there is no invented numerical tolerance. The input-observer revision additionally
records the actual Right/Left/Up/Down/Z/X/R state received by the game.

## Workloads and observed baseline

| Scenario | Executed evidence / baseline observation |
| --- | --- |
| full-jump | Existing floor with nine clear rows; grounded before Z. Rise 120 pixels, apex at frame 25, accepted jump at 10, landing at 44: 34 update intervals |
| short-jump | Release at frame 13; rise 53.000061 pixels, apex at 18, landing after 19 update intervals |
| double-jump | Two accepted jumps at 10 and 18; third attempt rejected; maximum count 2 |
| movement | Ten Right updates +50 pixels; ten Left -50; simultaneous Left/Right cancel displacement while preserving sequential input handling; existing Up/Down controls exercised |
| wall | Right held for 40 updates against an existing tile-2 wall; x remains fixed |
| head | Rising contact against an existing tile-2 ceiling records an actual head-collision event |
| oneway | Existing tile-8 platform catches a falling player and allows positive upward velocity from below |
| shooting | Shots at frames 10,16,22,28,34; six-update cadence and five-bullet cap reached |
| shoot-jump | Actual X+Z first-frame shot takes precedence over jumping, preserving the existing else-if chain |
| spikes | Actual tile-5 collision, more than 45 dead/UI-visible frames, hidden player; R clears the UI and respawns |
| checkpoint | Actual player bullet activates an existing checkpoint, a save event and counter-shot occur, and R restores the captured coordinates |
| transition | Actual tile-69/68 exits: room 1→2→1, including trap creation/update/clear and tile hashes |
| enemy | Bear encounter in room 4_1, including the existing alternating map phases |
| boss | 3300 updates in room 13; SPAWN, TYPEA, LIGHTATTACK, TYPEB and TYPEC all run, including projectile-hit/death state |
| boss-death | Separate 1200-update test using the existing God debug mode; real bullets reduce Boss HP to zero and existing victory platforms extend. This is a lifecycle/damage test, not proof of normal difficulty or a player victory |

Intervals above are simulation updates; at the nominal 1/60 step they correspond
to 0.567s full-jump and 0.317s short-jump. They are not independent measured wall
time or a frame-rate-independent physics claim. Existing per-frame coupling and
millisecond frame cap are retained. The earlier baseline report contains actual
wall-time observations, with their limitations.

Both numerical state parity and scenario coverage must pass: identical idle
traces do not count as a tested jump, shot, wall or Boss cycle. The runner checks
that the intended events, limits, collisions and transitions actually occurred.
Raw traces and per-case SHA-256s remain in `build/gameplay-regression/<Config>/`;
small checked-in reports summarize actual runs. Visual-pause runs are separately
named and excluded from numerical comparisons.

## Commands

Use the toolchain PATH from BUILD.md, then:

```powershell
python B:/scripts/run_gameplay_regression.py --cmake C:/Users/andyl/AppData/Local/Temp/boshy-tools/cmake-3.31.8-windows-x86_64/bin/cmake.exe --config Debug
python B:/scripts/run_gameplay_regression.py --cmake C:/Users/andyl/AppData/Local/Temp/boshy-tools/cmake-3.31.8-windows-x86_64/bin/cmake.exe --config Release
```

`--scenarios` selects focused cases; `--no-build` uses the already verified
executables. The runner refuses to treat a failed process as a passing trace.
Boss processes have a bounded 150-second timeout; other cases use the same
bound. A real graphics context is required. The normal game and 15 CTest cases
must also pass; this matrix is not compilation-only verification.

All test processes use SDL_AUDIODRIVER=dummy. Probes additionally force music,
channel and master volume to zero before App::Start, as requested on 2026-11-25.
Normal game sound/config defaults and Windows system volume are unchanged.

## Visual/native observations and scope

On 2026-11-25 the actual original and current death scenes were inspected through
Windows computer-use, paused after the same ten updates. Both displayed the
same Game Over / Press R overlay, hidden player, room-2 image and Debug controls.
The eight absent death frames are still absent; no invented replacements were
used. A first short visual pause expired before capture; a separately identified
longer pause was used for inspection and is not part of timing measurements.

The original native menu responded to Up. Computer-use Return/Enter taps did
not reliably enter; the held SDL event workloads entered both original/current
menus and exercised all gameplay bindings. Native physical keyboard feel is not
claimed as manually certified. For the numerical baseline gate the complete
actual-update/event comparisons are the automatic equivalent allowed by the
master task; no SDL/PTSD keyboard code or bindings changed. Subjective controller
feel and audible playback remain supplemental human checks, not fabricated PASSes.

The original/current matches establish the measured paths above, not every
possible level route, random seed, keyboard, audio device or graphics driver.
Future changes must rerun relevant original comparisons and keep open coverage
limits visible. A new unexplained physics/collision/timing drift blocks progression.

## Phase 2 additions

The full default matrix now includes `death-loop` (450 updates, three actual spike
deaths and three R restores). `--fps-cap 30|60|120` changes only a test-process
cap and writes a separate `<Config>-fps<cap>` directory. Reports include process
elapsed seconds and executable/source-manifest hashes. Elapsed values include
startup/shutdown and are not a frame-CPU profile. See PLAYER_REFACTOR.md for the
original frame-coupled clock decision and preserved unusual controls.

## Phase 3 additions

The default matrix now has 18 cases. `reload-stress` performs 60 real room
reloads over 600 updates; Current also verifies old checkpoint weak-owner expiry
and rejected teleport preservation. Original retained 60/60 checkpoints; Current
released 60/60. `spider-bird` spans 900 updates and observes spider pose/state,
map mutation, bird chase, rooms 8/9/12 and phase-8 timer persistence on re-entry.
God mode is declared for this lifecycle case. Test-only friends now include
spider/bird so their timers and coordinates are compared rather than inferred
from pointer presence. Source assets, masks and states are the original ones.
