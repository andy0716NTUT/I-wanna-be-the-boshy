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
fresh full19-case certification. Package/preview verification is recorded below.

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

## Corrected internal artifacts

Runtime commit, retained on remaster/continuous:
`7095ab71220366d793794a89bd332a6f7b8158b2`.

- Original-art RC3 executable:
  `build/release-candidate/BoshyRemaster-Windows-x64-RC3-7095ab71/IWannaBeTheBoshy.exe`.
- RC3 ZIP: `build/release-candidate/BoshyRemaster-Windows-x64-RC3-7095ab71.zip`,
  61361559 bytes, SHA256
  `9f8f6c1051e93cc2d4d8c73982b3e445a8076d7bd74af12008fa090bfd1d15cc`.
- Corrected scenery preview, silent launcher:
  `build/art-preview/Boshy-ArtPreview-v1-RC3-7095ab71/Play-ArtPreview-Silent.cmd`.
  Its adjacent `Play-Original-Assets-Silent.cmd` launches RC3 with original art.
  Both use the same corrected runtime, not the old RC2.
- Normal game executable SHA256 in RC3 and the new scenery preview:
  `0c2c8c38fde722dce0313ff66ebf830f1236da482aa4250764cebbb4de7c12e8`.

Package manifest,153 source resource copies, ZIP CRC/extraction and extracted
manifest pass. Normal Debug, packaged Release and Unicode-extracted Release
startup pass from C:/Windows with System32/Windows-only PATH and dummy audio.
Only original known missing phase1/death images occur. Evidence:
`evidence/checkpoint-reaction-package.json`.

The new scenery preview passes movement and corrected checkpoint replay with
every state exactly matching the fresh pre-art current-runtime traces. Its
native tuning suite passes reaction30 updates/travel8, sine385, stone support,
fall/reload and exact idle/gone pixels against its staged new art. Normal-game
startup passes from the Unicode path/unrelated cwd/minimal PATH. All31 original
PNG backups are verified; the candidate art, source Resources, old RC2 and old
preview are retained. Both old EXEs still have their previous SHA256
`b8d203116e2b2a92bcf025b4fdd8424d6a23c19f7761cfd21f61ee82b6fb5fdf`.
Evidence: `evidence/checkpoint-reaction-art-preview.json`. The old9-case art
report remains historical; this correction replays2 targeted themed cases.

Exact artifact commands, executed after committing the verified runtime:

```powershell
python B:/scripts/package_windows_rc.py --build-dir build/windows --runtime-licenses 'C:/Program Files/JetBrains/CLion 2025.3.3/bin/mingw/licenses' --objdump 'C:/Program Files/JetBrains/CLion 2025.3.3/bin/mingw/bin/objdump.exe' --name RC3
python B:/scripts/create_art_preview.py --base B:/build/release-candidate/BoshyRemaster-Windows-x64-RC3-7095ab71 --output B:/build/art-preview/Boshy-ArtPreview-v1-RC3-7095ab71 --evidence B:/build/checkpoint-reaction-fix/art-preview --checkpoint-directory B:/build/gameplay-tuning/Release-checkpoint-reaction --scenarios movement checkpoint
```

Keep adjacent Resources/PTSDAssets/config/DLLs together. Old RC2 and initial art
preview are deliberately unchanged; use the new paths above for the correction.
These are local internal artifacts; no game binaries were publicly uploaded.

## Limits and next review

Earlier firing intentionally changes checkpoint difficulty and can aim at a
different player location. The frame-coupled clock and endpoint projectile
collision remain, so low render FPS slows simulation and corner cases still
need human feel review. No physical keyboard/audio or all-hardware/all-routes
claim is made. Scenery taste/seams remain a separate manual review.

Smallest next PR: the checkpoint delay/travel correction and its scoped evidence
against the remaster branch; keep scenery candidates optional. No main merge,
force push, binary upload or public game release is performed.
