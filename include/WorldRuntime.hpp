#pragma once

// Original per-run event state. In-game R/room reloads retain these values;
// reset points remain explicit in AppUpdate rather than being inferred here.
struct WorldRuntime {
    float switchTimer = 0.0f;
    bool isSwitch = false;
    bool keepExtending = true;
    int startTileX = 28;
    int startTileY = 56;
    float platformTimer = 0.0f;
    bool phase8Entered = false;
    float phase8EnterTimer = 0.0f;
    float birdDelay = 0.0f;
};
