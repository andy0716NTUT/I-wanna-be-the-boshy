#include "ImageRegion.hpp"
#include <stdexcept>

ImageRegion::ImageRegion(const std::string& path, int x, int y, int width, int height)
    : m_Size(width, height),
      m_Program("PTSDAssets/shaders/Base.vert", "PTSDAssets/shaders/Base.frag"),
      m_Matrices(m_Program, "Matrices", 0) {
    const auto fail = [&path](const std::string& reason) {
        return std::runtime_error("Image region '" + path + "': " + reason);
    };
    const std::unique_ptr<SDL_Surface, decltype(&SDL_FreeSurface)> source(
        IMG_Load(path.c_str()), SDL_FreeSurface);
    if (!source) throw fail(IMG_GetError());
    if (x < 0 || y < 0 || width <= 0 || height <= 0 ||
        x > source->w - width || y > source->h - height)
        throw fail("crop outside image bounds");
    const std::unique_ptr<SDL_Surface, decltype(&SDL_FreeSurface)> crop(
        SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_RGBA32), SDL_FreeSurface);
    if (!crop) throw fail(SDL_GetError());
    SDL_Rect region{x, y, width, height};
    if (SDL_SetSurfaceBlendMode(source.get(), SDL_BLENDMODE_NONE) != 0 ||
        SDL_BlitSurface(source.get(), &region, crop.get(), nullptr) != 0)
        throw fail(SDL_GetError());
    m_Texture = std::make_unique<Core::Texture>(GL_RGBA, width, height, crop->pixels, false);
    m_Quad.AddVertexBuffer(std::make_unique<Core::VertexBuffer>(
        std::vector<float>{-.5f,.5f, -.5f,-.5f, .5f,-.5f, .5f,.5f}, 2));
    m_Quad.AddVertexBuffer(std::make_unique<Core::VertexBuffer>(
        std::vector<float>{0,0, 0,1, 1,1, 1,0}, 2));
    m_Quad.SetIndexBuffer(std::make_unique<Core::IndexBuffer>(
        std::vector<unsigned int>{0,1,2, 0,2,3}));
    m_Program.Bind();
    glUniform1i(glGetUniformLocation(m_Program.GetId(), "surface"), 0);
}

void ImageRegion::Draw(const Core::Matrices& data) {
    m_Matrices.SetData(0, data);
    m_Texture->Bind(0);
    m_Program.Bind();
    m_Quad.Bind();
    m_Quad.DrawTriangles();
}
