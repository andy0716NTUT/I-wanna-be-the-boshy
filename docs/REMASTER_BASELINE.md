# Remaster behavior baseline

Measured baseline gate: **PASSED on 2026-11-25** using the expanded actual-game
event matrix in [GAMEPLAY_REGRESSION.md](GAMEPLAY_REGRESSION.md). Debug and Release
each passed all 15 cases, including grounded jumps, collisions, bullets,
checkpoint/death/respawn, room re-entry, enemy, full Boss cycle and defeat lifecycle.
The older partial evidence and its limits below are retained for history.
This is measured-equivalent automated acceptance, not a claimed human playthrough.

Original source: `eeb1f72c6c3a9a9e33a49c31946e4b723323d49f`.
Verified Windows Phase 1: `8947ffd6b3d78bb34cdefd5d01be11de05fc80f8`.
Machine: Windows 10.0.26200 x64, GCC 13.1.0, Ninja 1.12.1, CMake 3.31.8,
RTX 5060, OpenGL 4.1 NVIDIA 621.24; native config 1280x960, fps_cap 60.

## Confirmed source paths

`src/main.cpp` calls `App::Start`, then `App::Update` and `Core::Context::Update`.
The active player code is in AppUpdate. PlayerManager.cpp is not in the executable
source list and there is no active PlayerManager construction/call. It contains
different proposed collision/input logic; do not enable it as a cosmetic cleanup.
Character's private physics fields are not the active movement owner.

Source observations, not measured gameplay promises:

- Default App Gravity is -0.5. AppUpdate adds it at function entry, including menu
  frames, then adds it again in the gameplay branch. Preserve both additions.
- Active horizontal movement is +/-5 per update; independent left/right checks
  and existing up/down +/-5 controls are intentional baseline facts to preserve.
- JumpPower 16 and MaxFallSpeed -10 are per-update values, not time-scaled units.
  Early release multiplies positive vertical velocity by 0.4. Jump count, death
  and the shooting `else if` chain affect whether jump input takes effect.
- Shoot cooldown decreases by deltaTime*4 at entry and another deltaTime while
  alive; it resets to 0.5 and the vector cap is five. Actual shot cadence/count
  has not been measured by the current probe.
- The nominal deltaTime constant is 1/60. PTSD polls input after App updates,
  swaps buffers, and applies its existing millisecond delay. No timestep change
  has been made and no low/high render-FPS parity gate has passed.
- Coordinates use +/-640, +/-480 and 16-pixel tiles. Map indexing and World's
  ragged layout contain unusual conventions; tests do not authorize changing them.
- Pinned Util::Image defaults to useAA=true. Existing filtering was retained in
  Phase 1; Phase 4 nearest/integer scaling work has not started.

## Automated measurements actually run

The opt-in `BoshyOriginalProbe` compiles the original Git versions of all game
translation units and headers used by the original game target. The original
files are extracted byte-for-byte, without editing, under ignored
`build/original-probe-source`. `BoshyPhase1Probe` compiles current source.
Both use identical pinned PTSD, audited original assets and the common test entry.

Build deviations for the original probe: test main replaces original main;
resource/shader paths use the verified executable-relative deployment; CMake is
the recovered configuration, not original CMake; no player/physics statements
are replaced or mocked. The test calls App::Start, then performs 320 App updates,
each followed by the real Context::Update. This is a graphical integration test,
not a complete native-keyboard playthrough or identical startup-frame timing.

SDL key-down/up events persist across explicit update frames, avoiding short OS
tap ambiguity. Schedule events are enqueued after App update N and consumed by
Context so they affect update N+1:

| Key | Down event frame | Up event frame |
| --- | --- | --- |
| Return | 0 / 20 | 2 / 22 |
| Right | 50 | 60 |
| Z | 80 / 140 | 100 / 143 |
| X | 180 | 240 |
| R | 260 | 262 |
| Left | 280 | 290 |

Both Debug and Release comparisons passed. Each pair contains 320 rows and zero
differences in logged phase, world, coordinates, tile and key state. Comparisons
exclude elapsed wall time. Existing DebugInfo strings use default decimal stream
precision; this is not a bitwise comparison of all internal floating-point state.
An injected one-pixel drift at frame 100 was correctly rejected by the comparator.

| Observation | Original Debug / Phase 1 Debug | Original Release / Phase 1 Release |
| --- | --- | --- |
| First non-menu frame | 21 / 21 | 21 / 21 |
| Ten Right updates (pre-update samples 51→61) | +50 x, 0 y / identical | +50 x, 0 y / identical |
| First Z window: initial logged y | 209 / 209 | 209 / 209 |
| First Z window: largest logged y | 319 at frame 97 / identical | 319 at frame 97 / identical |
| Second Z window: initial / largest y | -66 / -66, no rise in window, identical | identical |
| Right / Left / Z / X / R held update counts | 10 / 10 / 23 / 60 / 2, identical | identical |
| Observed mean update interval (ms) | 16.975 / 17.109 | 16.856 / 16.625 |
| Observed maximum interval (ms) | 85.686 / 79.317 | 45.927 / 51.092 |

These intervals include loading/entry and normal scheduling, from one run each.
They are not performance or timing-parity claims. The first Z occurs while
already falling: frame 82 logs y=199, then the trajectory rises to y=319. Thus
110 is the rise from the pre-input sample, not a clean grounded jump-height
measurement. The second Z has no rising trajectory; jump count and death state
are not recorded, so its cause and short-hop behavior are not certified.
Do not change gravity or loosen expected values to make this a simpler arc.

Raw evidence: `docs/evidence/scene-{debug,release}-{original,phase1}.csv`,
`docs/evidence/scene-comparison.json`, `docs/evidence/scene-release-comparison.json`.
Each probe exited 0. Runtime logs remain under ignored `build/baseline/<Config>/`.

Exact reproducible comparison commands after the Windows build:

```powershell
$env:PATH = 'C:\Program Files\JetBrains\CLion 2025.3.3\bin\mingw\bin;C:\Program Files\JetBrains\CLion 2025.3.3\bin\ninja\win\x64;' + $env:PATH
python B:/scripts/run_scene_comparison.py --cmake C:/Users/andyl/AppData/Local/Temp/boshy-tools/cmake-3.31.8-windows-x86_64/bin/cmake.exe --config Debug
python B:/scripts/run_scene_comparison.py --cmake C:/Users/andyl/AppData/Local/Temp/boshy-tools/cmake-3.31.8-windows-x86_64/bin/cmake.exe --config Release
```

The script verifies original source extraction, builds the two excluded probe
targets, runs them sequentially with a 20-second timeout each, then compares.
An OpenGL-capable Windows desktop is required; failure is not headless success.
Probes are OFF by default, excluded from normal builds, and never linked into the
normal game. Turn off with `-DBOSHY_BUILD_SCENE_PROBES=OFF` for ordinary builds.

## Original resources and missing assets

153 Resources files were verified against original Git bytes. 28 TXT files have
pre-existing CRLF conversion from Windows checkout; all image/audio/font bytes
are identical. The manifest records both original and initial checkout hashes.
All 24 original maps and all four trap/spider masks match their original parser
outputs cell-for-cell. Invalid files are now diagnosed instead of silently
substituting cells or indexing a short row; valid numeric tile meanings are intact.

Original AppStart eagerly references death1..death4.png and their -left variants
under Character/death; all eight are absent. BackgroundImage's initial
Image/Background/phase1.png is also absent. Upstream logs these paths and uses its
existing missing-texture fallback; both observed menus still render. No assets
were synthesized, fetched, replaced or renamed. Missing resources are not hidden.

GameOverUI Hide clears its drawable; Show reloads Image/dead.png and resets its
timer. AppUpdate calls Show in the real-death branch and hides the player. This
is only a source-path audit: death graphics, breathing timing and R behavior
require a graphical death/respawn comparison before any fix.

## Initial gate gaps and their resolution

At the 2026-11-24 checkpoint Stage 1.5 was NEEDS_MANUAL_VERIFICATION. Gaps included a clean
grounded full/short/second jump and airtime, wall/head/one-way/spike collisions,
active bullet count and cadence, checkpoint activation and correct respawn,
room/world transitions, trap/enemy lifecycle, Boss1 timing and existing Debug UI.
The current coordinates and input stream do not expose the internal events needed
to certify these. The native-keyboard menu attempts did not advance with short
automation taps; the SDL held-key probe does enter successfully. No input change
is justified by that difference.

The subsequent 2026-11-25 work added test-only read-only event/state capture for
velocity, ground/death state, bullets, checkpoint and phase transitions, plus
controlled workloads and original comparisons. Those event/physics cases now pass;
original/current death UI and Debug rendering were also visually inspected.
Native menu Up worked, while short Return taps remained unreliable through the
automation tool. Physical keyboard feel and audible playback are supplemental
human checks; they are not claimed as manually passed. The master task permits
measured equivalent critical paths; no native input/backend code changed.
Phase 2 can now use the recorded exact-state comparisons without altering
gravity, update cadence, jump rules, collision formulas or shooting priority.

An interactive `BoshyOriginalGame` target uses the unchanged original main and game
sources with build/deployment shims only. Build it in Debug or Release, then run
from its own deployment directory. It has not been manually played. For a
concrete side-by-side validation session (run one executable at a time):

```powershell
& 'C:/Users/andyl/AppData/Local/Temp/boshy-tools/cmake-3.31.8-windows-x86_64/bin/cmake.exe' -S B:/ -B B:/build/windows '-DBOSHY_BUILD_SCENE_PROBES=ON' '-DBOSHY_ORIGINAL_SOURCE_DIR:PATH=B:/build/original-probe-source'
& 'C:/Users/andyl/AppData/Local/Temp/boshy-tools/cmake-3.31.8-windows-x86_64/bin/cmake.exe' --build B:/build/windows --config Debug --target BoshyOriginalGame --parallel 8
Start-Process 'B:/build/windows/bin/Debug/BoshyOriginalGame.exe' -WorkingDirectory 'B:/build/windows/bin/Debug'
Start-Process 'B:/build/windows/bin/Debug/IWannaBeTheBoshy.exe' -WorkingDirectory 'B:/build/windows/bin/Debug'
```
