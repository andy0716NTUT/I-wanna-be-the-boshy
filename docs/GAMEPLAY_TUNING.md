# Requested gameplay tuning after RC1

Reference runtime/tooling: `b28f807b1f3e71e7ce7be40dd649ff3de8392009`.
These are explicit user-approved exceptions to the original-game parity contract,
not a claim that the original behavior was defective. The latest correction is
authoritative: **one spike formation emerges while the other retreats**, rather
than both approaching the player together. All validation uses dummy audio.

## Room 2

Both formations already used `sin(time * 2)`. The floor's initial Y/stroke are
`-480 / 480`, and the ceiling's are `160 / 190`. They now consume one App-owned
room clock, reset at trap creation. Both have the same world-Y sign, which makes
their intrusion from opposite screen edges alternate. Origins, strokes, period
(pi simulation seconds), rounding and collision-mask updates remain unchanged.
This deliberately introduces **no new faster stroke or opposite sine sign**.
The change prevents independent clocks from diverging if their update paths
change later; the previous simultaneously updated clocks were already in phase.

## Room 9

The right-hand grassy stone before the heart blocks was baked into phase9.png,
with ordinary solid tiles at columns50–55, rows44–47. It was not a tile7
FallingGround object. Its center is `(208,-256)`, size96x64.

Only WORLD1/9 receives a TippingStone. The existing platform.png's opaque pixels
exactly match this stone. A runtime crop from existing phasenone.png covers its
original rectangle; a separate platform sprite renders above that cover. No PNG,
TXT, audio, source background, tile ID or other room geometry is rewritten.

Landing on the top while descending triggers it once. The loaded side sinks
first about the opposite upper corner: right load -> clockwise, left load ->
counterclockwise. The angle eases quadratically to24 degrees over a0.32-second
timer (20 updates at the retained60-Hz simulation step), then the tilted stone
accelerates downward and continues rotating. It stops supporting the player when
the free fall begins and retires its own sprite below the screen. The stationary
mountain cover remains until the room object is retired.

On activation, only original solid1/2 cells within that24-cell footprint are
cleared. One-way support follows the actual tilted top during tipping and keeps
the existing12-pixel player clearance; jumping can leave the surface. R or
same-room object reload restores the exact saved footprint. Leaving the room
removes the sprite and cover together. The original tile collision applies
unchanged until activation. This room-specific contact and timing change needs
human feel review; it does not modify global player physics or hitboxes.

## Checkpoint counter-shot

Travel changes from8 to16 world units per update globally. The original1-second
firing delay, aiming snapshot/zero-distance fallback, projectile lifetime,
save delay, R position, player bullet speed15, firing cadence and cap stay intact.
The faster shot intentionally hits sooner and can cross a tile boundary sooner.
The existing endpoint-based wall/player collision policy is retained; this is
not a new swept-collision implementation or a guarantee about every corner.

## Reproduce

Use the compiler/CMake/Ninja paths and ASCII B: mapping in BUILD.md. First run
`python B:/scripts/prepare_regression_sources.py`, configure with the documented
`windows_build.py` wrapper, then build the excluded targets:

```powershell
$cmake = 'C:/Users/andyl/AppData/Local/Temp/boshy-tools/cmake-3.31.8-windows-x86_64/bin/cmake.exe'
$env:PATH = 'C:/Program Files/JetBrains/CLion 2025.3.3/bin/mingw/bin;' + $env:PATH
& $cmake --build B:/build/windows --config Debug --target BoshyOriginalRegression BoshyCurrentRegression GameplayTuningProbe --parallel 8
python B:/scripts/run_gameplay_tuning.py --config Debug
python B:/scripts/verify_checkpoint_tuning.py --config Debug
```

Repeat for Release. Optional `run_gameplay_tuning.py --size 1080p` or `--size 4k`
verifies the new layer and original-region pixels at larger physical sizes.
The tuning runner requires the existing Pillow installation. It exports the
game's actual GL backbuffer, not the desktop, while hiding its own test window.

The existing strict `run_gameplay_regression.py` comparator is unchanged. Run
all18 scenarios except `checkpoint` with `--no-build`; the checkpoint scenario
is measured separately with explicit8/16-speed expectations and unchanged
save/firing/R checks. Do not label the new checkpoint trajectory as original
parity or weaken that comparator to conceal an authorized difference.

## Verification evidence

All tests run on the Windows/GCC13.1/CMake3.31.8/Ninja/RTX5060 environment in
BUILD.md, with dummy audio. Tests execute real App updates and rendered frames;
no claim of human keyboard/audio testing is made.

| Check | Outcome | Evidence |
| --- | --- | --- |
| Debug/Release normal build and CTest | PASS,20/20 each | build/windows/{build,ctest}-{debug,release}.log |
| Debug original/current unchanged scenarios | PASS,18/18 | evidence/gameplay-tuning-parity-debug.json |
| Release original/current unchanged scenarios | PASS,18/18 | evidence/gameplay-tuning-parity-release.json |
| New real-scene checks, native Debug/Release | PASS | evidence/gameplay-tuning-{debug,release}.json |
| Native/1080p/4K idle and gone pixels | PASS,exact original regions | native reports and evidence/gameplay-tuning-{1080p,4k}.json |
| Shared sine positions | PASS,385 updates vs original formula | build/gameplay-tuning/*/tuning.csv |
| Actual stone landing/tipping/fall | PASS,first tip7 / free fall26 / gone62 | native trace + reviewed idle/tipping/falling/gone PNGs |
| One-way slope support and jump departure | PASS,19 supported tipping updates; jumping leaves surface | GameplayTuningProbe assertions |
| R,12 same-room reloads and room re-entry | PASS,exact original tiles restored and old object expired | GameplayTuningProbe assertions |
| Counter-shot speed, aim, lifetime, save and R | PASS,both configurations | evidence/gameplay-tuning-checkpoint-{debug,release}.json |
| Original153 resources | PASS,no content drift;28 known checkout CRLF conversions | evidence/gameplay-tuning-assets.json |

The checkpoint fixture fires at update65 in both versions. Speed is8→16;
the unobstructed player hit changes update70→67. This difference is intentional.

Actual commands used, with the same tool paths in each configuration:

```powershell
$cmake = 'C:/Users/andyl/AppData/Local/Temp/boshy-tools/cmake-3.31.8-windows-x86_64/bin/cmake.exe'
$toolchain = 'C:/Program Files/JetBrains/CLion 2025.3.3/bin/mingw/bin'
$ninja = 'C:/Program Files/JetBrains/CLion 2025.3.3/bin/ninja/win/x64/ninja.exe'
$env:PATH = "$toolchain;" + $env:PATH
python B:/scripts/prepare_regression_sources.py
# Existing build/windows has BOSHY_BUILD_REGRESSION_PROBES=ON.
foreach ($configuration in @('Debug','Release')) {
    python B:/scripts/windows_build.py --cmake $cmake --toolchain $toolchain --ninja $ninja --config $configuration --offline
    & $cmake --build B:/build/windows --config $configuration --target BoshyOriginalRegression BoshyCurrentRegression GameplayTuningProbe --parallel 8
    python B:/scripts/run_gameplay_tuning.py --config $configuration
    python B:/scripts/verify_checkpoint_tuning.py --config $configuration
    python B:/scripts/run_gameplay_regression.py --cmake $cmake --config $configuration --no-build --scenarios full-jump short-jump double-jump movement wall head oneway shooting shoot-jump spikes transition enemy boss boss-death death-loop reload-stress spider-bird resize-play
}
python B:/scripts/run_gameplay_tuning.py --config Release --size 1080p
python B:/scripts/run_gameplay_tuning.py --config Release --size 4k
python B:/scripts/audit_baseline.py
git diff --check
```

One direct CMake reconfigure without the documented wrapper reproduced this
computer's known0xC0000409 program-search crash. The wrapper recovered cleanly;
no system setting/tool installation changed. An initial image assertion found
12 player-sprite pixels overlapping the idle crop; moving only the test player
below that region restored the exact comparison. No tolerance was introduced.

Remaining review: human playtest of the requested feel/difficulty, especially
stepping near the stone edges and checkpoint wall corners. The original
frame-coupled simulation and endpoint projectile collision remain; low render
FPS still slows game time. Suggested smallest next PR: these three requested
tunings, their scoped scene tests and this report, against the remaster branch.
No PR, main merge or public release is created by this task.
