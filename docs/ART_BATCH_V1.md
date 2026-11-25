# Reversible scenery redraw batch v1

The user approved the mountain redraw, requested other materials with originals
retained and rollback available, then explicitly selected **backgrounds, terrain
and scene props; keep characters and animations original**. This is the first
scenery candidate set, not a replacement of the source Resources directory.

## Checkpoints and files

- Pre-art source/runtime: `1b2738855b8221348ec1e6835309e4bbbfeb66d9`.
- Pushed annotated rollback tag: `remaster-before-assets-20261126-1b27388`.
- Approved reference/inventory backup commit: `651fd3b` (pushed).
- `art/remaster-v1/reference/world1-mountain-approved.png`: approved raw style
  reference, retained unchanged and backed up in Git.
- `art/remaster-v1/source-inventory.json`:119 original PNG names, hashes,
  dimensions, alpha hashes and bounds.
- `art/remaster-v1/raw/`:7 new unedited built-in image_gen outputs: two
  backdrops, two props and three terrain textures. Prompts are in `prompts/`;
  raw PNG hashes are in `raw-manifest.json`. No CLI image-model fallback used.
- `art/remaster-v1/candidates/manifest.json`:31 source-file mappings, output and
  source hashes, protection/material masks, template-match counts and reasons.
- `art/remaster-v1/candidates/Image/`:27 changed scenery PNG candidates and4
  deliberately preserved original PNGs, all at original canvas sizes/alpha.
- `art/remaster-v1/candidates/materials/`:3 separate normalized material tiles.
- `art/remaster-v1/candidates/review/index.html`: offline original/candidate
  comparison page; click an image for its actual PNG. `review/overview.png`
  shows representative room9, pink boss scenery and World2 comparisons.
- `art/remaster-v1/verification.json`: independent checks and existing-runtime
  CTest results. `.gitattributes` keeps prompt/manifest line endings stable.

`scripts/build_art_candidates.py` assembles these files offline from the saved
raw outputs; it does not regenerate AI images or need an API key. It restricts
output to workspace art/build directories, refuses different existing variants,
and checks original hashes before and after writing. `verify_art_candidates.py`
independently verifies every mapped PNG, mask and input checksum.

## Method and boundaries

The actual new artwork was made with built-in image_gen using original asset
references and the approved mountain style. Mechanical Pillow/NumPy processing
then normalizes dimensions with nearest-neighbor, trims model-added padding in
derived copies and restores prop alpha exactly. Raw outputs are never rewritten.

Baked map PNGs mix scenery and gameplay art. Mountain rooms use the known old
backdrop; pink/World2 backgrounds use map-guided empty-pixel consensus. Geometry
neighborhoods prevent old cloud/heart fragments being retained as foreground.
Visible white/gray spike landmarks and non-painted protected pixels remain exact.
These segmentation masks are approximate visual masks, never collision data.

New prop textures replace only patches positively matched against the original
prop's opaque pixels. Stone/topsoil textures replace only matched source patches
inside existing solid tiles; green microtexture changes only saturated green
interiors inside solid platform rectangles. Existing coordinates and sizes stay
fixed. Unmatched foreground stays original; this is not a claim that every
terrain pixel has been repainted. Sprite/cover/baked-image coherence for room9's
tipping stone is checked pixel-for-pixel.

Four images are intentionally byte-identical: both moving spike formations and
the two World2 `for collision` reference PNGs. Room7's old backdrop differs
enough that automatic separation left seams; its backdrop is held original,
with only positively matched terrain/props updated. Trial folders1–4 under
ignored build output are historical attempts; only the versioned candidate set
is the handoff. No bad trial is promoted as accepted final artwork.

## Actual checks

```powershell
python B:/scripts/build_art_candidates.py --output B:/art/remaster-v1/candidates
python B:/scripts/verify_art_candidates.py --manifest B:/art/remaster-v1/candidates/manifest.json
python B:/scripts/audit_baseline.py
$env:SDL_AUDIODRIVER = 'dummy'
$env:PATH = 'C:/Program Files/JetBrains/CLion 2025.3.3/bin/mingw/bin;' + $env:PATH
$ctest = 'C:/Users/andyl/AppData/Local/Temp/boshy-tools/cmake-3.31.8-windows-x86_64/bin/ctest.exe'
& $ctest --test-dir B:/build/windows -C Debug --output-on-failure
& $ctest --test-dir B:/build/windows -C Release --output-on-failure
git diff --exit-code remaster-before-assets-20261126-1b27388 -- src include Resources CMakeLists.txt files.cmake config.json
git diff --check
```

PASS:119 original PNGs and153 total original resources have no content drift
(28 historical TXT CRLF conversions retained). All31 mapped canvases/alpha
match originals. Checked5,094,271 protected pixels and2,179,501 mapped material
pixels, visible spike landmarks and exact room9 stone-cover coherence. Negative
controls with wrong canvas and alpha are rejected. Existing original-runtime
Debug/Release CTest each20/20 PASS (2.13s/1.39s), with dummy audio. C++ source,
Resources, config and build definitions are unchanged from the rollback tag.

The old temporary CMake copy was missing. An attempted configure with the existing
`.tools`3.31.8 copy reproduced0xC0000409. Restoring the existing portable tool
to its recorded Temp path allowed both CTest runs to pass; no system install.
No new C++ compilation or candidate-loaded gameplay parity is claimed by those
CTest results. Separate candidate-runtime validation is recorded below if run.

## Review and rollback

The source game's default assets and existing RC2 remain original. Candidate
art is a separate directory and is not copied over Resources by this tool.
The pre-art tag preserves the exact code/original assets; later candidate
commits preserve the new images and recipes. Nothing requires deleting either
version or force-pushing. For a parallel view of the pre-art tree, a developer
can use a new worktree, e.g.:

```powershell
git worktree add --detach B:/build/pre-art-checkpoint remaster-before-assets-20261126-1b27388
```

Remaining manual checks: visual seams, foreground readability, material repetition
and taste across every room; room7 backdrop needs deliberate registration/masking.
Characters, animation frames, UI, fonts and audio are outside this chosen batch.
No public game release/main merge. The smallest next PR is this separate art
candidate/validation set; default-theme integration needs its own rendered-game
and gameplay acceptance rather than assuming that pixel-size checks prove parity.

## Isolated playable preview and runtime evidence

Art checkpoint `0c8878d7e5d807a3d54f54d89210c6acfe3e1e58` is pushed and tagged
`remaster-art-v1-scenery-20261126`. `scripts/create_art_preview.py` creates a new
ignored build directory from the accepted RC2 package. It verifies the RC2
manifest, preserves every replaced PNG under `OriginalAssets/`, and overlays
only candidate PNGs inside that new copy. The unchanged game executable still
has SHA256 `b8d203116e2b2a92bcf025b4fdd8424d6a23c19f7761cfd21f61ee82b6fb5fdf`.
No source Resources or existing RC2 file is replaced.

Executed command:

```powershell
python B:/scripts/create_art_preview.py --output B:/build/art-preview/Boshy-ArtPreview-v1-0c8878d
```

For interrupted verification of the same complete stage, add `--resume`; verified
replay cases are retained by trace checksum. The helper uses the existing RC2,
Release probes and accepted trace oracles under build/. Those are prerequisites
from the prior regression work, not newly compiled by this script. A different
probe/oracle checksum is rejected; it never silently updates the pre-art oracle.

PASS on the actual candidate-loaded runtime:9 Release replay cases, each exactly
matching all pre-art logged states (movement, full jump, shooting, spikes,
checkpoint, transitions, spider/bird, resize and3300-update Boss cycle). The real
themed tipping probe also passes385 sine updates, landing/tip/fall/jump departure,
R plus12 reloads and cleanup. Idle/gone stone backbuffer regions exactly match
the themed staged PNGs. The normal game executable starts from its Unicode path,
C:/Windows cwd and minimal PATH. All agent runs use dummy audio. Temporary copied
probe executables are removed only after their owned test processes exit; source
probe binaries, all assets and the preview game are retained.

Evidence: `art/remaster-v1/runtime-verification.json`; actual game backbuffer:
`art/remaster-v1/candidates/review/ingame-stone-tipping.png`. The screenshot uses
God mode only as a declared test fixture; the normal preview executable retains
the original default controls/state. These checks do not certify every World2
route, every room's visual taste, physical keyboard/audio or performance on all
hardware. No new C++ compilation is claimed.

To play, open
`build/art-preview/Boshy-ArtPreview-v1-0c8878d/Play-ArtPreview-Silent.cmd`.
To return to the retained original RC2 in this workspace, close the preview and
open `Play-Original-RC2-Silent.cmd` in that folder. Both launchers force silence
only for their child game. The old RC2 path and repository default remain intact;
every original staged PNG is additionally recoverable from OriginalAssets.
The preview is local/internal, not a public release or automatic default switch.
