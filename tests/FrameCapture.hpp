#pragma once
#include "pch.hpp"
#include <filesystem>
#include <fstream>
#include <stdexcept>

// Test-only backbuffer artifact, not a desktop/window screenshot. No source asset edits.
inline void CaptureGameFrame(const std::filesystem::path& output) {
    int width, height;
    SDL_GL_GetDrawableSize(SDL_GL_GetCurrentWindow(), &width, &height);
    if (width <= 0 || height <= 0) throw std::runtime_error("Cannot capture empty game drawable");
    GLint framebuffer, pack, readBuffer;
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING,&framebuffer);
    if (framebuffer != 0) throw std::runtime_error("Game capture expected presented window framebuffer");
    glGetIntegerv(GL_PACK_ALIGNMENT,&pack);
    glGetIntegerv(GL_READ_BUFFER,&readBuffer);
    glReadBuffer(GL_BACK);
    glPixelStorei(GL_PACK_ALIGNMENT,1);
    std::vector<unsigned char> pixels(static_cast<std::size_t>(width)*height*3);
    glReadPixels(0,0,width,height,GL_RGB,GL_UNSIGNED_BYTE,pixels.data());
    glPixelStorei(GL_PACK_ALIGNMENT,pack);
    glReadBuffer(readBuffer);
    if (glGetError() != GL_NO_ERROR) throw std::runtime_error("OpenGL game capture failed");
    std::ofstream file(output,std::ios::binary);
    file << "P6\n" << width << ' ' << height << "\n255\n";
    for (int y=height-1;y>=0;--y)
        file.write(reinterpret_cast<const char*>(pixels.data()+static_cast<std::size_t>(y)*width*3),width*3);
    if (!file) throw std::runtime_error("Cannot save game frame: "+output.u8string());
}
