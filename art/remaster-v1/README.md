# Remaster art v1 — separate, reversible candidates

The user approved the reference-composition mountain redraw and requested the
same treatment for other visual assets, preserving originals and a rollback.

- Pre-art source/runtime checkpoint: `1b2738855b8221348ec1e6835309e4bbbfeb66d9`.
- Pushed annotated tag: `remaster-before-assets-20261126-1b27388`.
- `source-inventory.json` records all119 original PNG paths, dimensions, file
  hashes, alpha bounds and alpha hashes before this batch.
- `reference/world1-mountain-approved.png` is the approved1448x1086 redraw,
  copied without edits from the local preview. This commit backs up the actual
  accepted PNG as well as the previously pushed prompt/provenance.
- Original153 Resources files, existing TXT maps and RC2 are retained. No file
  in this directory is loaded by the current game by default.

New work is stored only under this versioned directory with explicit source
mapping, source/output hashes, prompts and validation status. Raw model output
and normalized game-sized candidates must be distinguished. A generated image
is not automatically approved or collision/animation equivalent.

The default game remains the original asset set, making this candidate batch
non-destructive. For a separate checkout of the exact pre-art code/assets,
use the pushed checkpoint tag; preserve any working-tree changes first. Do not
reset or force-push remaster/continuous. A later runtime theme switch, if added,
must keep the original option and safe fallback.

## Scenery batch

The user selected backgrounds, terrain and scene props, retaining characters
and animations. `candidates/review/index.html` is the offline comparison page;
`candidates/review/overview.png` is a compact before/after sheet.
`candidates/manifest.json` maps31 original scenery files:27 changed candidates
and4 intentionally unchanged critical/data images, plus3 material tiles.
Room7 keeps its original backdrop pending registration review; only matched
terrain/props are updated there. Source Resources are never overwritten.

See `../../docs/ART_BATCH_V1.md` in the repository for commands, exact limits and
rollback details. Build/verification scripts are under `scripts/`. All new art,
raw model outputs and prompts remain versioned separately from the originals.

An isolated playable preview is under
`build/art-preview/Boshy-ArtPreview-v1-0c8878d/` in the development workspace.
`Play-ArtPreview-Silent.cmd` opens the new art; `Play-Original-RC2-Silent.cmd`
opens the retained old RC2. The new directory backs up every overlaid PNG in
OriginalAssets. Nine exact themed replay cases and real tipping/render checks
pass; see runtime-verification.json. This does not switch the repository default.
