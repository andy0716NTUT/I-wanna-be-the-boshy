# Player refactor and clock boundary

`App` owns one `PlayerManager`. Its initialized `PlayerRuntimeState` replaces the
four function statics in the active update path (vertical velocity, jump count,
shoot cooldown and jump-release state). The old, never-compiled raw-pointer
PlayerManager proposal was not the gameplay oracle: it omitted UP/DOWN, used
mutually exclusive direction inputs and a different gravity/cooldown sequence.
The replacement extracts the active `AppUpdate.cpp` behavior instead.

`App` still owns scene/death/checkpoint state and bullet lifetimes. Movement input
is sampled once per axis, in Right/Left/Up/Down order. Tile sampling remains before
vertical integration; collision response uses the original cached contacts and
480/16/4 coordinate expressions. No new reset is added to R or room transitions.
Player state resets with a new App instance, while an in-game respawn retains it
as in the original. The unsupported case of two simultaneous Apps now has distinct
player states rather than sharing function statics.

The two gravity additions, -10 fall clamp, jump velocity 16, 0.4 early release,
5 units per directional update, independent simultaneous-key branches, 5-bullet
cap, common cooldown -= dt*4 then alive cooldown -= dt, 0.5 shot reset, and X
priority over Z all remain explicit. Debug gravity and God mode remain owned by
App and passed into these operations.

## Clock decision

The stock main invokes one App update and one `Core::Context::Update` per frame.
PTSD v0.3-rc1 caps that loop using a cached `1000/FPS_CAP` millisecond target and
integer SDL_Delay. Gameplay always passes `1.0f/60.0f`, irrespective of wall time.
Thus frame stalls and non-60 caps change real-time gameplay speed in the original.
Moving these units into a wall-time accumulator would change that behavior and
also animation/input polling order. This refactor preserves the original clock;
`PlayerManager::FrameSeconds` names its existing value. No accumulator or render
interpolation is introduced. A new fixed simulation scheduler remains conditional
on separate low/high-FPS wall-time parity evidence and explicit review.

The opt-in real-game harness supports `--fps-cap 30|60|120`, applies the cap only
to the test process before the engine caches it, and records process elapsed
seconds. This includes startup/shutdown and is not a CPU-frame microbenchmark.
Every harness process uses dummy audio plus zero channel/music/master volume.

## Validation protocol

Run the complete Debug/Release matrix in GAMEPLAY_REGRESSION.md after changes.
The 16th case `death-loop` visits an existing spike, waits 90 updates, presses R,
and repeats three times over 450 updates. Require three observed respawns and UI
hidden after each R. Match all original/current recorded fields as exact decimal
representations of floats; no numeric tolerance is used. The source generator
places head/floor observers inside the extracted function for Current and keeps
the original in-place observers for Original. Production builds contain neither.

Observed cap probes (three 150-update cases, including startup/shutdown): cap30
processes take about 6.4s, cap120 about 2.1s. Original/current state is exact in
all six cases. These confirm the existing cap-dependent clock, not wall-time
equivalence of a new scheduler. Full Phase 2 test reports are in docs/evidence.
