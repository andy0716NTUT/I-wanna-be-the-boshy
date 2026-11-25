# Mountain background quality preview

Requested one-image trial, 2026-11-25 Asia/Taipei. This follows the proposed
traditional image-processing approach: no AI repaint or newly fetched asset.
It is a separately named review candidate, not a game resource replacement.

Source: `Resources/Image/Background/WORLD1/phasenone.png`,1280x960 RGBA.
Original SHA256: `c8807e255f6b75fd55958db6c06ac2a3f4ab2441eaccbdbd1578f7f91ab0747a`.
Candidate SHA256: `e63170f9d962ad6ad954dae4b723776e9208351726852977cfcd5bf0ce1abfa3`.

## Files and rationale

- `scripts/preview_background_quality.py`: reproducible Pillow-only processing,
  candidate export, comparison sheets, validation and hashes. It refuses an
  existing output directory to preserve previous reviews.
- `build/asset-preview/world1-mountain-v1-final/phasenone-quality-v1.png`:
  full-size candidate. Contrast1.055, saturation1.025, broad local contrast
  unsharp radius16/12%/threshold4, edge unsharp radius1.8/65%/threshold3.
  No resize; original alpha retained.
- `comparison.png` in the same directory: original left/candidate right, matching
  half-size overviews and native100% detail crops.
- `room-comparison.png` and `phase9-background-mockup-v1.png`: static context
  illustration only. Baked map/background colors differ by up to5 levels even
  in plain sky; the mockup uses a maximum-channel difference8 background mask.
  All protected pixels outside this approximate mask are unchanged. This is
  not a real game screenshot or a production foreground-extraction pipeline.
- `docs/evidence/asset-preview-world1-v1.json`: parameters, checks and PNG hashes.
  Candidate PNGs are local ignored review artifacts, not committed replacements.

## Verification and outcome

Commands actually run:

```powershell
python B:/scripts/preview_background_quality.py --output B:/build/asset-preview/world1-mountain-v1-final
python B:/scripts/audit_baseline.py
git diff --check
```

PASS: candidate dimensions1280x960 and alpha unchanged; original background and
room hashes unchanged; protected context pixels unchanged; original153 resources
have no content drift (28 historical TXT checkout CRLF conversions retained).
Reviewed both final sheets visually. The first exact-match context mask showed
rounding-noise stripes and was rejected; the tolerance8 static mockup removes
that artifact without changing the actual background candidate. Final output is
the `-final` directory; the earlier working directory is not the chosen preview.

Build/CTest/gameplay execution: NOT_RUN for this preview; no runtime source,
resource binding, collision, filtering, map or release package changes. No game
or audio process was started. The previous RC2 verification is historical evidence,
not a newly claimed test result.

## Limits and next step

The source is already strongly blurred. This treatment slightly strengthens
contrast/edges; it does not recover true missing detail or create HD texture.
Its darkness/edge halos and foreground readability need taste review before any
deployment. The context mask is approximate and must not become game collision
data. No gameplay-facing change occurred.

Smallest next PR, only if this direction is selected: an opt-in background
candidate with consistent treatment across baked room backgrounds and the room9
stone cover, plus real rendered scene checks. Replacing only phasenone.png would
make the room9 cover disagree with its baked background, so that alone is not a
valid rollout. No bulk replacement or public asset publication in this task.
