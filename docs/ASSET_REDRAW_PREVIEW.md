# Reference-composition mountain redraw preview

The user explicitly selected "依原構圖重新繪製，一張試試看效果" after
reviewing the limited traditional-processing result. This authorizes this one
AI-assisted redraw preview; it does not deploy a replacement or authorize bulk
asset changes. Built-in image_gen was used, with the original local background
as the single edit reference and transparent_background=false. No CLI fallback.

Source: `Resources/Image/Background/WORLD1/phasenone.png`,1280x960.
Output: `build/asset-preview/world1-mountain-redraw-v1/phasenone-redraw-v1.png`,
1448x1086,4:3 PNG. The generated file was copied unchanged into the workspace;
the source and default generated-file copy were retained. The requested ideal
size was1280x960; the model returned1448x1086. No resampling or post-edit was
performed. This is a review image, not a correctly sized drop-in game resource.

Source SHA256: `c8807e255f6b75fd55958db6c06ac2a3f4ab2441eaccbdbd1578f7f91ab0747a`.
Candidate SHA256: `3f3faf2d7bafe9abf304ab5df27ce8b0d8d14092c649a350ac0b51834e0fed8c`.
Exact final prompt: [asset-redraw-world1-v1-prompt.txt](evidence/asset-redraw-world1-v1-prompt.txt).
Metadata/checks: [asset-redraw-world1-v1.json](evidence/asset-redraw-world1-v1.json).

## Visual outcome and limits

The preview visibly replaces blurry rock patches with deliberate ridges and
pixel clusters, and redraws teal palms and pale ribbed plants. Overall sky,
left-center mountain grouping and lower vegetation arrangement are similar;
some silhouettes, small-peak locations and plant details differ. This is newly
inferred artwork, not restoration of the exact lost original texture. No
pixel-alignment/gameplay equivalence is claimed. The result was visually
inspected from the built-in tool output. No second variant was requested.

## Validation

PASS: PNG decoding via Pillow Image.open(...).verify(), actual1448x1086 size,
exact4:3 aspect ratio and unchanged source SHA256. `python B:/scripts/audit_baseline.py`
PASS:153 original assets unchanged,28 historical TXT CRLF conversions retained.
`git diff --check` PASS.

Debug/Release builds, CTest and gameplay tests: NOT_RUN for this independent
image preview. No source code, original asset, collision data, shader/filter,
resource binding or RC2 package changed; no game/audio process launched.

## Next smallest change

Review this visual direction first. A later opt-in integration would need
1280x960 geometry or an explicitly correct display transform, a coherent
background layer for all baked room images and the room9 stone cover, preserved
collision maps, and real rendered-scene checks. This task provides one local
PNG candidate only. Generated binary artwork stays in ignored build output;
the prompt/provenance/progress documentation is committed for resumption.
