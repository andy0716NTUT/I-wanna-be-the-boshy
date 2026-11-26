# Weighted falling feel — optional v1 preview

User request on2026-11-26: the character feels light/floaty and gravity lacks
weight; exact desired tuning is not yet known. This authorizes a reversible
feel experiment. It does not establish subjective approval or level-wide
playability. Original remains the default, including when old config files
omit the new option. Old RC3 and scenery preview stay unchanged.

## Candidate

| Parameter | Original | Weighted candidate |
| --- | --- | --- |
| Jump launch |16 units/update|16, unchanged|
| Rising gravity, total of two additions |-1/update|-1, unchanged|
| Falling gravity, total of two additions |-1/update|-1.6/update|
| Maximum downward travel |10/update|14/update|
| Jump release multiplier |0.4|0.4, unchanged|
| Horizontal travel |5/update|5, unchanged|
| Jump limit |2|2, unchanged|

Gravity is selected once at the beginning of the existing App update. With
negative Debug gravity and non-positive starting velocity, Weighted multiplies
it by1.6 for both historical gravity additions. An ascent that crosses zero
during the update still uses the original gravity. No hover/pause, scheduler,
input buffer, coyote time, acceleration-based horizontal movement or new reset
is introduced. Zero/positive Debug gravity retain their original values.

Normal single-jump launch, ascending samples, apex and early release are intended
to remain exact; descent accelerates sooner and reaches a higher downward cap.
Switching to Original restores the original cap/gravity branch. R retains the
selected profile just as it retains player state; switching does not clear
cooldowns, velocities, jump count, saves or room state.

## Use and rollback

Optional config field: `"player_feel": "original"` or `"player_feel": "weighted"`.
Wrong types/unknown values fail with the configuration path/reason. The shipped
source config is unchanged and defaults to Original. Debug Window → Controls →
Player feel switches Original/Weighted immediately, with no hotkey changes.
The Debug choice lasts for that process; restarting reads config again.

Separate silent preview launchers and exact paths will be recorded after gates
pass. The weighted folder retains its unmodified base config as
config-original.json and all original-asset backups when staging from scenery.
The Original launcher uses the same executable revision and same scenery with
the original profile, so comparison does not depend on changing pictures.

## Changed files

- PlayerManager.hpp/cpp: explicit Original/Weighted profile, once-per-update
  gravity selection and profile-specific fall limit.
- AppUpdate.cpp, AppStart.cpp, App.hpp, imgui.cpp: actual update integration,
  config initialization and visible reversible profile switch.
- RuntimeConfig.hpp and RuntimeConfigTests.cpp: optional validated choice,
  old-file default, valid/invalid type/value coverage.
- RegressionProbe.cpp/run_gameplay_regression.py: additional actual-map128-unit
  drop fixture; original strict equality and existing expected values retained.
- GameplayTuningProbe.cpp: exact rising-arc comparison, terminal limits,
  Debug gravity and return-to-Original checks, alongside real-scene tests.
- verify_player_feel.py: isolated weighted runtime, fresh binary/trace-hash
  oracles, measured arcs and collision/lifecycle checks; no parity relaxation.

## Verification checkpoint

Debug/Release normal/probe builds pass;20/20 CTest each(1.58s/1.48s), including
config choice/default/error tests. Both configurations pass15/15 exact
Original-profile/eeb1f72 replays, separate corrected checkpoint measurement,
native tuning checks and14/14 Weighted game scenarios. The full jump's ascending
samples are exact with the same120-unit rise;34→30 airborne updates, landing44→40.
Tap jump preserves53.000061-unit rise;19→17 airborne updates, landing29→27.
The128-unit drop is raised at10 and lands27→23 on the same original floor;
measured terminal velocity is-10→-14. Horizontal inputs, shooting cadence/cap,
wall/head/one-way contact, spikes/death/R, checkpoint wait30/travel8,
room transitions and resize/fullscreen/restoration checks pass.

Weighted actual room9 landing starts tipping5 instead of7 because the approach
is faster; free fall starts24 instead of26, retirement60 instead of62. The
19-update supported tip duration,12 reloads/R/re-entry, jumping off the slope,
385 sine positions and exact idle/gone pixels pass in both configurations.
Both profile integrators and return-to-Original checks pass. Normal Weighted
EXEs start from Unicode paths/unrelated cwd/minimal PATH with dummy audio.
Source153 Resources have no content drift;28 old checkout CRLF conversions remain.

Fresh evidence is docs/evidence/player-feel-v1-{original,weighted,tuning}-
{debug,release}.json plus checkpoint/assets reports. The test Git parent is
31777be, and each trace report pins the tested binary/source-manifest hashes.
The implementation is committed after these gates; that committed runtime and
separate scenery preview are recorded below. Human feel approval remains
NEEDS_MANUAL_VERIFICATION. No preferred final feel or full-route approval is claimed.

Reproduce with B: mapping and the existing pinned toolchain in BUILD.md:

```powershell
$cmake = 'C:/Users/andyl/AppData/Local/Temp/boshy-tools/cmake-3.31.8-windows-x86_64/bin/cmake.exe'
$ctest = 'C:/Users/andyl/AppData/Local/Temp/boshy-tools/cmake-3.31.8-windows-x86_64/bin/ctest.exe'
$env:PATH = 'C:/Program Files/JetBrains/CLion 2025.3.3/bin/mingw/bin;' + $env:PATH
$env:SDL_AUDIODRIVER = 'dummy'
python B:/scripts/prepare_regression_sources.py
foreach ($configuration in @('Debug','Release')) {
    & $cmake --build B:/build/windows --config $configuration --parallel 8
    & $cmake --build B:/build/windows --config $configuration --target BoshyOriginalRegression BoshyCurrentRegression GameplayTuningProbe --parallel 8
    & $ctest --test-dir B:/build/windows -C $configuration --output-on-failure
    python B:/scripts/run_gameplay_regression.py --cmake $cmake --config $configuration --no-build --scenarios full-jump short-jump double-jump fall movement wall head oneway shooting shoot-jump spikes transition death-loop resize-play boss-death
    python B:/scripts/verify_checkpoint_tuning.py --config $configuration
    python B:/scripts/run_gameplay_tuning.py --config $configuration
    python B:/scripts/verify_player_feel.py --config $configuration
}
```

The existing configured CMake cache is reused; this task does not claim a new
clean configure. Logs are in build/player-feel-v1; raw traces are kept in build/.
Tests use hidden real SDL/OpenGL windows and dummy audio, not simulated physics
alone or a claimed native-keyboard/human playthrough.

## Risks and next review

Earlier landing reduces horizontal jump reach even though single-jump height
is unchanged. A delayed second jump may start from a lower position and reach
a different total height. Spike/Boss timing windows and landing on a moving or
tilting platform may feel harder; all routes are not certified. Collision
hitboxes, tile semantics, pre-integration cached contact lookup and endpoint
rules are unchanged. A14-unit cap remains below16px per tile, but this alone is
not proof of collision correctness; actual fixtures are required.

The frame-coupled simulation remains: stalls/lower FPS slow real-time gameplay.
No claim that this preview fixes that separate timing limitation. Human review
should compare one held jump, a tap jump, double jump and a gap crossing with
both profiles. Subjective acceptance is NEEDS_MANUAL_VERIFICATION; do not make
Weighted the default without that review. Smallest next PR: this optional player
feel candidate and its tests only, separate from default asset integration.
No PR/main merge/force push/public game release is created here.
