# Checkpoint reaction correction — 2026-11-26

The user clarified that "twice as fast" means the wait from a checkpoint being
hit to its counter-shot. The earlier0b15f6bf implementation instead doubled
projectile travel. That interpretation was wrong. RC2 and the initial scenery
preview retain it and are historical artifacts, not the corrected build.

## Current behavior and implementation

- `include/MapObject/CheckPoint.hpp`: reaction threshold1.0 →0.5 simulation seconds.
- `src/Bullet.cpp`: restore checkpoint travel16 → original8 world units/update.
- Save delay, animation100ms/frame, aiming at firing time, zero-target fallback,
  lifetime, reset-on-repeat-hit behavior and R semantics are retained. Player
  shooting, movement, hitboxes and both earlier room2/room9 tunings are unchanged.
- Test-only generated observers now record actual checkpoint bullet activation
  in both original/current App updates. They are absent from the normal game.
- `GameplayTuningProbe` checks the30-update boundary, inactive/reset/repeated-hit
  behavior,8-unit travel, fallback, lifetime and offscreen removal. The dedicated
  verifier checks real player-bullet activation, firing, save/R and aim/lifetime.
- The isolated art-preview helper accepts a verified base RC, separate evidence
  and checkpoint oracle directory. Per-case probe hashes and resume runtime/path
  checks prevent using the old runtime's traces for the corrected preview.

## Measured results

Debug and Release actual game fixture,150 updates each version:

| Measurement | Original eeb1f72 | Corrected runtime |
| --- | --- | --- |
| Activation frame |4|4|
| Counter-shot frame |65|34|
| Hit-to-shot updates |61|30|
| Delay threshold, simulation seconds |1.0|0.5|
| Travel per update |8|8|
| Player hit frame, unobstructed fixture |70|39|
| Save timing/position and R |PASS|PASS|
| Aim direction and initial lifetime |PASS|PASS|

At the retained1/60 step, float accumulation reaches1.0 after61 additions and
0.5 after30. This is the original accumulator with only its threshold changed;
no timestep/collision reordering or comparison tolerance conceals that difference.
The reaction is deliberately earlier, so this is not original timing parity.

Debug/Release normal and opt-in probes build successfully. CTest passes20/20
in each configuration (2.10s /1.92s). Both native real-GL tuning suites pass:
385 alternating sine updates, stone tip7/fall26/gone62,19 supported tip updates,
jump departure, R/12 reloads/re-entry and exact idle/gone rendered regions.
The original153 Resources files have no content drift;28 historical checkout
CRLF conversions remain. No source PNG, audio, TXT map or art candidate is edited.

Strict original/current comparison passes10/10 unchanged scenarios in both
Debug and Release: full-jump, movement, wall, oneway, shooting, spikes,
transition, boss-death, spider-bird and resize-play. Every recorded state field
matches exactly; the strict comparator is unchanged. This focused run is not a
fresh full19-case certification. New package/preview verification follows this
implementation checkpoint and will be recorded below.

## Reproduce

Use the existing B: workspace mapping and pinned local toolchain in BUILD.md.
No system installation or global audio/config change is required. All game
test runners use SDL dummy audio and hidden windows.

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
    python B:/scripts/verify_checkpoint_tuning.py --config $configuration
    python B:/scripts/run_gameplay_tuning.py --config $configuration
    python B:/scripts/run_gameplay_regression.py --cmake $cmake --config $configuration --no-build --scenarios full-jump movement wall oneway shooting spikes transition boss-death spider-bird resize-play
}
python B:/scripts/audit_baseline.py
git diff --check
```

The existing configured build cache was reused; clean configure is not claimed
for this two-value correction. Build/CTest logs are in build/checkpoint-reaction-fix
and build/windows/ctest-{debug,release}.log. Full traces/images are under ignored
build/gameplay-tuning and build/gameplay-regression. Small immutable evidence
copies are recorded as docs/evidence/checkpoint-reaction-*.json.

## Limits and next review

Earlier firing intentionally changes checkpoint difficulty and can aim at a
different player location. The frame-coupled clock and endpoint projectile
collision remain, so low render FPS slows simulation and corner cases still
need human feel review. No physical keyboard/audio or all-hardware/all-routes
claim is made. Scenery taste/seams remain a separate manual review.

Smallest next PR: the checkpoint delay/travel correction and its scoped evidence
against the remaster branch; keep scenery candidates optional. No main merge,
force push, binary upload or public game release is performed.
