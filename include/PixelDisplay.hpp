#pragma once
#include "DisplayViewport.hpp"
struct SDL_Window;
namespace Util { class Renderer; }

// Owns only presentation resources. No world/input/physics state is scaled.
class PixelDisplay {
public:
    PixelDisplay() = default;
    ~PixelDisplay();
    PixelDisplay(const PixelDisplay&) = delete;
    PixelDisplay& operator=(const PixelDisplay&) = delete;
    void Initialize();
    void BeginFrame();
    void Draw(Util::Renderer& renderer);
    bool SetFullscreen(bool enabled);
    bool IsFullscreen() const;
    std::optional<Display::Point> WindowToLogical(float x, float y) const;
    Display::Point LogicalToWindow(Display::Point p) const;
    Display::Viewport GetViewport() const { return m_Viewport; }
    int GetDrawableWidth() const { return m_Width; }
    int GetDrawableHeight() const { return m_Height; }
private:
    void RefreshSize();
    SDL_Window* m_Window = nullptr; // SDL/Context owns the window.
    unsigned int m_Framebuffer = 0, m_Color = 0, m_Depth = 0, m_Sampler = 0;
    int m_Width = 0, m_Height = 0;
    int m_WindowedWidth = 1280, m_WindowedHeight = 960;
    Display::Viewport m_Viewport;
};
