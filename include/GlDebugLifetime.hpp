#pragma once
#include "pch.hpp"

// Construct after Context, before App/display. PTSD's global Context outlives
// spdlog's registry; a late driver message must not call the destroyed logger.
// This guard changes shutdown only, and leaves runtime GL diagnostics enabled.
class GlDebugLifetime {
public:
    ~GlDebugLifetime() {
#ifndef __APPLE__
        if (SDL_GL_GetCurrentContext() && glDebugMessageCallback)
            glDebugMessageCallback(nullptr, nullptr);
#endif
    }
};
