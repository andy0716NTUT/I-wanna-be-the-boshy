#include "GlDebugLifetime.hpp"
#include "Core/Context.hpp"
#include "PixelDisplay.hpp"
#include "RuntimePaths.hpp"
#include "Util/Renderer.hpp"
#include <fstream>
#include <iostream>
#include <cmath>
#include <stdexcept>

namespace {
void Check(bool ok, const char* message) { if (!ok) throw std::runtime_error(message); }
class Stripes : public Core::Drawable {
public:
    glm::vec2 GetSize() const override { return {1280,960}; }
    void Draw(const Core::Matrices&) override {
        GLint sampler = 0, filter = 0, viewport[4];
        glGetIntegeri_v(GL_SAMPLER_BINDING,0,&sampler);
        Check(sampler != 0,"Image draw has no nearest sampler");
        glGetSamplerParameteriv(sampler,GL_TEXTURE_MAG_FILTER,&filter);
        Check(filter == GL_NEAREST,"Magnification is smoothed");
        glGetSamplerParameteriv(sampler,GL_TEXTURE_MIN_FILTER,&filter);
        Check(filter == GL_NEAREST,"Minification is smoothed");
        glGetIntegerv(GL_VIEWPORT,viewport);
        Check(viewport[2] == 1280 && viewport[3] == 960,"World viewport changed");
        glEnable(GL_SCISSOR_TEST);
        for (int x=0;x<1280;++x) {
            glScissor(x,0,1,960);
            glClearColor(x%2 ? 0 : 1,x%2 ? 1 : 0,0,1);
            glClear(GL_COLOR_BUFFER_BIT);
        }
        glDisable(GL_SCISSOR_TEST);
    }
};
void Verify(PixelDisplay& display) {
    display.BeginFrame();
    GLint framebuffer;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING,&framebuffer);
    Check(framebuffer == 0,"Early-return frame would clear the logical buffer instead of window");
    Util::Renderer root;
    root.AddChild(std::make_shared<Util::GameObject>(std::make_shared<Stripes>(),0));
    display.Draw(root);
    const int width=display.GetDrawableWidth(),height=display.GetDrawableHeight();
    const auto viewport=display.GetViewport();
    std::vector<unsigned char> pixels(static_cast<std::size_t>(width)*height*3);
    glPixelStorei(GL_PACK_ALIGNMENT,1);
    glReadPixels(0,0,width,height,GL_RGB,GL_UNSIGNED_BYTE,pixels.data());
    const auto checkPixel=[&](int x,int y,int red,int green) {
        const auto i=(static_cast<std::size_t>(y)*width+x)*3;
        if (!(pixels[i] == red && pixels[i+1] == green && pixels[i+2] == 0)) {
            std::cerr << "Sample " << width << 'x' << height << " at " << x << ',' << y
                      << " got " << int(pixels[i]) << ',' << int(pixels[i+1]) << ',' << int(pixels[i+2])
                      << " expected " << red << ',' << green << '\n';
            throw std::runtime_error("Pixel sample/letterbox mismatch");
        }
    };
    if (viewport.x > 0) { checkPixel(0,height/2,0,0); checkPixel(width-1,height/2,0,0); }
    if (viewport.y > 0) { checkPixel(width/2,0,0,0); checkPixel(width/2,height-1,0,0); }
    for (int x=0;x<viewport.width;++x) {
        const int numerator=(2*x+1)*1280, denominator=2*viewport.width;
        const int sourceX=numerator/denominator;
        // At a fractional downscale boundary the two source pixel centers are
        // equally near. Either exact source color is nearest; mixing is forbidden.
        if (numerator % denominator == 0) {
            const auto i=(static_cast<std::size_t>(viewport.y+viewport.height/2)*width+viewport.x+x)*3;
            Check(pixels[i+2] == 0 && ((pixels[i] == 255 && pixels[i+1] == 0) ||
                  (pixels[i] == 0 && pixels[i+1] == 255)),"Nearest tie blended source colors");
        } else {
            checkPixel(viewport.x+x,viewport.y+viewport.height/2,sourceX%2 ? 0 : 255,sourceX%2 ? 255 : 0);
        }
    }
    int windowWidth,windowHeight;
    SDL_GetWindowSize(SDL_GL_GetCurrentWindow(),&windowWidth,&windowHeight);
    const auto center=display.WindowToLogical(windowWidth/2.0f,windowHeight/2.0f);
    Check(center && std::abs(center->x)<1 && std::abs(center->y)<1,"Window/DPI mouse center mismatch");
    Check(glGetError() == GL_NO_ERROR,"Display produced a GL error");
    std::cout << "PASS " << width << 'x' << height << " content " << viewport.width << 'x' << viewport.height << '\n';
}
}
int main(int, char**) try {
    SDL_setenv("SDL_AUDIODRIVER","dummy",1);
    const std::unique_ptr<char,decltype(&SDL_free)> base(SDL_GetBasePath(),SDL_free);
    Check(bool(base),"No executable directory");
    UseRuntimeDirectory(std::filesystem::u8path(base.get()));
    auto context=Core::Context::GetInstance();
    const GlDebugLifetime glDebugLifetime;
    Mix_MasterVolume(0); Mix_Volume(-1,0); Mix_VolumeMusic(0);
    PixelDisplay display;
    display.Initialize();
    auto* window=SDL_GL_GetCurrentWindow();
    for (const auto size : {std::pair{1280,960}, {1920,1080}, {2560,1440}, {3840,2160}, {960,540}}) {
        SDL_SetWindowSize(window,size.first,size.second);
        SDL_PumpEvents();
        Verify(display);
        Check(display.GetDrawableWidth() == size.first && display.GetDrawableHeight() == size.second,
              "Requested resolution not tested on actual drawable");
    }
    Check(display.SetFullscreen(true) && display.IsFullscreen(),"Cannot enter desktop fullscreen");
    SDL_PumpEvents(); Verify(display);
    Check(display.SetFullscreen(false) && !display.IsFullscreen(),"Cannot restore windowed mode");
    SDL_PumpEvents(); Verify(display);
    return 0;
} catch (const std::exception& error) {
    std::cerr << error.what() << '\n'; return 1;
}
