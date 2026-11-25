# Pixel display integration

The existing game world is 1280x960: 80x60 tiles, 16 units each, centered at
(+/-640,+/-480). PixelDisplay renders exactly that logical surface into a fixed
RGBA8/depth24 framebuffer. PTSD's original projection reads the same native
logical dimensions. SDL's physical window size comes from window_width and
window_height in config.json and can be resized; it no longer changes world
coordinates. The default remains the original 1280x960 window.

No PTSD source/asset changes: a scene-scoped OpenGL sampler on the image texture
unit (0) selects GL_NEAREST for minification and magnification; the prior sampler
is restored before ImGui. A GL_NEAREST framebuffer blit presents the canvas.
HUD, text, sprites and animations inside the scene keep their original transforms,
anchors and sizes. Debug ImGui is drawn afterward in physical window coordinates.
There is no simulation scheduler, animation interpolation or collision scaling.

## Scaling and settings

| Physical drawable | Content | Centered bars |
| --- | --- | --- |
| 1280x960 | 1280x960 (1x) | none |
| 1920x1080 | 1280x960 (1x) | 320 left/right, 60 top/bottom |
| 2560x1440 | 1280x960 (1x) | 640 left/right, 240 top/bottom |
| 3840x2160 | 2560x1920 (2x) | 640 left/right, 120 top/bottom |
| 960x540 | 720x540 (fractional downscale) | 120 left/right |

Fit uses the largest whole native multiple when the window fits the canvas;
smaller windows use nearest fractional downscaling with an exact 4:3 viewport.
It never adds visible world area or stretches to widescreen. Window minimum is
320x240. An optional boolean `fullscreen` in config.json starts desktop fullscreen;
the default is false. Existing Debug controls include a Fullscreen checkbox which
returns to the previous window size. No gameplay keyboard bindings changed.

Display::ToLogical rejects points in bars. PixelDisplay::WindowToLogical converts
SDL window points through drawable/window DPI ratio before mapping to the canvas;
LogicalToWindow supplies the inverse for overlays. ImGui keeps its own SDL physical
mouse handling. Any future world-mouse feature must use this mapper, not the
upstream window-center-only Util::PTSDPosition conversion.

## Frame lifetime

BeginFrame clears the logical color/depth buffer once per App update (or Start),
then restores the default framebuffer. Each former Root.Update call renders into
that canvas and presents it. Repeated same-frame respawn draws keep the same
logical depth history; early gameplay returns leave the window bound for the
existing Context swap/clear. No world state is reset on resize/fullscreen.

The new GL readback workload reproduced an engine teardown defect: a late NVIDIA
performance message during global Core::Context destruction called a destroyed
spdlog logger (GDB stack, exit 0xC0000005). A local GlDebugLifetime guard in normal
and test entry points unregisters the callback after App/display destruction but
before global teardown. Runtime GL errors/warnings remain enabled. The pinned
engine remains byte-for-byte unchanged. This is shutdown-only, not a gameplay
parity exception. Raw debugger logs remain in ignored build/.

## Validation checkpoint

DisplayViewportTests passes aspect, integer scale, odd sizes, zero/minimized sizes,
bar rejection and coordinate round trips. PixelDisplayTests is an opt-in real-GL
target (not a desktop-dependent CTest): actual drawable sizes native/1080p/1440p/4K,
small-window, desktop fullscreen and restored-window all pass in Debug. It checks
scene sampler min/mag, native GL viewport, every pixel of a one-pixel stripe row,
black bar edges, mouse center, GL errors and early-return framebuffer binding.
Readback causes a driver synchronization performance warning; it is not a GL error.

`resize-play` uses actual original/current App updates while moving/jumping/shooting
and changing the SDL window among those sizes/fullscreen. Debug first run passes
exact state comparison. Remaining full Debug/Release, graphical scene inspection,
all CTest and asset audit are pending before the Phase 4 gate can pass.

Interruption checkpoint: Debug full gameplay 19/19 now passes in two reports;
Debug and Release GL display tests pass. Release full gameplay and all 18 CTest
remain pending. Real-game GUI review was not completed: the first short hold
expired, and the user stopped Computer Use with physical Escape during the next
attempt. That review is not marked PASS. See REMASTER_PROGRESS.md NEXT_ACTION.

## Resumed graphical verification

`python B:/scripts/capture_game_frames.py` exports the actual App + ImGui GL_BACK
buffer at the declared frame through test-only FrameCapture.hpp. It avoids desktop
capture/focus entirely. Original/current native, current 1080p, 4K and jumping
player outputs were inspected; game geometry/HUD, pixel edges and bars are correct.
The 1080p logical canvas and 4K canvas reduced by exact nearest 2x replication are
byte-identical. Native/current vs 1080p has zero differences across 1,146,971 pixels
outside the physical Debug window rectangle [59,59,231,531]. Debug panel text clips
at its old saved 170px width; this cosmetic pre-existing panel constraint is Phase 5
UI work. Hashes/procedure: docs/evidence/phase4-scene-review.json; generated PNGs
under ignored build/visual-review. These are rendered artifacts, not human keyboard
feel or physical audio-device certification.

Both configurations now pass 18/18 CTest and all seven real-GL target/restore cases.
Fractional sampling also checks every stripe-row output pixel. The initial stricter
test picked only the upper neighbor at an exact source-boundary tie (960x540,
destination x=124, source x=8); actual output selected the equally near lower
neighbor. The corrected test accepts either unmixed nearest color only at such
geometric ties, still checks exact colors elsewhere, and does not relax any
physics/state comparison. [Khronos interpolation reference](https://raw.githubusercontent.com/KhronosGroup/OpenGL-Refpages/main/gl4/glBlitFramebuffer.xml).

Final Phase 4 gate: PASS. Release full 19-case matrix now passes, matching Debug
19/19; both normal builds and 18/18 CTest pass. All earlier pending/interruption
notes above are historical checkpoints, resolved by the rendered-frame method.
