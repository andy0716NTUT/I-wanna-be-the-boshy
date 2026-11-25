# World and scene lifetime boundary

World layout, phase aliases, PNG/TXT files, tile IDs and conversion expressions
are unchanged. `World::GetPhaseAt(world,row,column)` now checks both dimensions
of the existing ragged layout and names invalid coordinates in its exception.
The historical column/row return order of GetStartPosition/GetPhaseIndex stays
compatible with all call sites. World 2 retains the original shared-layout
fallback; this is not a reconstruction of unfinished later worlds.

MapInfoLoader parses through one implementation. LoadMap still clears stale data
on failure as verified in Phase 1. The new TryLoadMap preserves the previous map
on failure. Debug teleport validates layout and image, then uses that operation
before mutating phase/player/scene state. Missing/invalid debug destinations are
rejected with their path. Invalid production world indexing now reports an
exception instead of reading outside the vector. These are deliberate error-path
changes, not changes to valid room transitions.

App owns WorldRuntime for the existing switch/platform/spider/bird timers. R and
room reloads keep the original explicit reset points. In particular the old
phase-8 else block wrote two different unused statics: it did not reset the active
entry flag/timer. The extraction preserves the active timer across exits. Enemy
bear statics remain unchanged until a separately measured reset boundary exists.

Retired platforms, checkpoints, boosts, falling ground, player bullets, spider,
bird and cleared Boss objects/projectiles are detached from Renderer as their
existing hide/clear operation occurs. Old inactive trap registrations are removed
before replacement. The invisible startup bear is registered once. Boss attack
math, counters, RNG, timers, collision and HP are unchanged. Expired Type-C shots
are detached at the same original x < -700 removal condition: their four source
frames are at most 67px wide, default scale1/rotation0, so their right edge is
already left of -666.5, outside the -640 content edge. No new pooling or callbacks.

## Verified evidence

Debug `reload-stress`: 60 real room reloads, exact original/current gameplay data.
Weak checkpoint owners expire 0/60 times in Original and 60/60 in Current. Each
Current reload also rejects two invalid debug destinations without changing the
active map hash or player position. `scene-lifetime` verifies release after
single/vector/duplicate registration and repeated cleanup over 500 cycles without
a GL window. `map-loader` and `world-lookup` pass malformed/transactional/bounds
cases and every original map/cell. New test main uses SDL_MAIN_HANDLED because it
is a console/headless test, not the SDL window entry point.

The new `spider-bird` workload observes all spider states, tile hashes, bird chase,
rooms 8/9/12 and immediate phase-8 re-entry. It matches exactly in Debug and Release. All 18 cases pass
in each configuration, and CTest is 17/17 each. Reports are phase3-*.json in
docs/evidence. Renderer retains its pinned
upstream equal-Z ordering; same-Z overlap screenshots remain part of visual QA.
