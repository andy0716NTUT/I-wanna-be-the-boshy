#include "PixelDisplay.hpp"
#include "Util/Renderer.hpp"
#include "config.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <stdexcept>

PixelDisplay::~PixelDisplay() {
    if (m_Sampler) glDeleteSamplers(1, &m_Sampler);
    if (m_Depth) glDeleteRenderbuffers(1, &m_Depth);
    if (m_Color) glDeleteTextures(1, &m_Color);
    if (m_Framebuffer) glDeleteFramebuffers(1, &m_Framebuffer);
}

void PixelDisplay::Initialize() {
    if (m_Framebuffer) return;
    m_Window = SDL_GL_GetCurrentWindow();
    if (!m_Window) throw std::runtime_error("Pixel display requires the current SDL/OpenGL window");
    // PTSD uses these for its projection only; SDL keeps the physical size from config.
    PTSD_Config::WINDOW_WIDTH = Display::LogicalWidth;
    PTSD_Config::WINDOW_HEIGHT = Display::LogicalHeight;
    SDL_SetWindowResizable(m_Window, SDL_TRUE);
    SDL_SetWindowMinimumSize(m_Window, 320, 240);

    glGenFramebuffers(1, &m_Framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
    glGenTextures(1, &m_Color);
    glBindTexture(GL_TEXTURE_2D, m_Color);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Display::LogicalWidth, Display::LogicalHeight,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Color, 0);
    glGenRenderbuffers(1, &m_Depth);
    glBindRenderbuffer(GL_RENDERBUFFER, m_Depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, Display::LogicalWidth, Display::LogicalHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_Depth);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("Cannot create complete 1280x960 pixel framebuffer");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glGenSamplers(1, &m_Sampler);
    glSamplerParameteri(m_Sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glSamplerParameteri(m_Sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // Sampler objects override Image/Animation texture filtering without patching PTSD.
    glSamplerParameteri(m_Sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glSamplerParameteri(m_Sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
    std::ifstream file("config.json");
    const auto config = nlohmann::json::parse(file);
    if (config.value("fullscreen", false) && !SetFullscreen(true))
        throw std::runtime_error(std::string("Cannot enter fullscreen: ")+SDL_GetError());
    RefreshSize();
}

void PixelDisplay::RefreshSize() {
    SDL_GL_GetDrawableSize(m_Window, &m_Width, &m_Height);
    m_Viewport = Display::Fit(m_Width, m_Height);
}

void PixelDisplay::BeginFrame() {
    RefreshSize();
    glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
    glViewport(0, 0, Display::LogicalWidth, Display::LogicalHeight);
    const auto scissor = glIsEnabled(GL_SCISSOR_TEST);
    glDisable(GL_SCISSOR_TEST);
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (scissor) glEnable(GL_SCISSOR_TEST);
    // Early gameplay returns must leave Context's swap/clear on the window.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, m_Width, m_Height);
}

void PixelDisplay::Draw(Util::Renderer& renderer) {
    glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
    glViewport(0, 0, Display::LogicalWidth, Display::LogicalHeight);
    GLint oldSampler = 0;
    glGetIntegeri_v(GL_SAMPLER_BINDING, 0, &oldSampler);
    glBindSampler(0, m_Sampler);
    renderer.Update();
    glBindSampler(0, static_cast<GLuint>(oldSampler));
    RefreshSize();
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_Framebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    const auto scissor = glIsEnabled(GL_SCISSOR_TEST);
    glDisable(GL_SCISSOR_TEST);
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (m_Viewport.width > 0 && m_Viewport.height > 0) {
        glBlitFramebuffer(0,0,Display::LogicalWidth,Display::LogicalHeight,
            m_Viewport.x,m_Viewport.y,m_Viewport.x+m_Viewport.width,m_Viewport.y+m_Viewport.height,
            GL_COLOR_BUFFER_BIT,GL_NEAREST);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, m_Width, m_Height); // ImGui remains in physical window coordinates.
    if (scissor) glEnable(GL_SCISSOR_TEST);
}

bool PixelDisplay::IsFullscreen() const {
    return m_Window && (SDL_GetWindowFlags(m_Window) & SDL_WINDOW_FULLSCREEN_DESKTOP) != 0;
}
bool PixelDisplay::SetFullscreen(bool enabled) {
    if (!m_Window) return false;
    if (enabled == IsFullscreen()) return true;
    if (enabled) SDL_GetWindowSize(m_Window, &m_WindowedWidth, &m_WindowedHeight);
    if (SDL_SetWindowFullscreen(m_Window, enabled ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0) != 0) return false;
    if (!enabled) SDL_SetWindowSize(m_Window, m_WindowedWidth, m_WindowedHeight);
    RefreshSize();
    return true;
}
std::optional<Display::Point> PixelDisplay::WindowToLogical(float x, float y) const {
    int w, h;
    SDL_GetWindowSize(m_Window, &w, &h);
    if (w <= 0 || h <= 0) return std::nullopt;
    return Display::ToLogical(m_Viewport,m_Height,x*m_Width/w,y*m_Height/h);
}
Display::Point PixelDisplay::LogicalToWindow(Display::Point p) const {
    int w, h;
    SDL_GetWindowSize(m_Window, &w, &h);
    const auto point = Display::ToDrawable(m_Viewport,m_Height,p);
    if (m_Width <= 0 || m_Height <= 0) return {};
    return {point.x*w/m_Width, point.y*h/m_Height};
}
