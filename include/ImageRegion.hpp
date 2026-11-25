#pragma once

#include "Core/Drawable.hpp"
#include "Core/Program.hpp"
#include "Core/Texture.hpp"
#include "Core/UniformBuffer.hpp"
#include "Core/VertexArray.hpp"

// A runtime crop of an existing image. Source files are never rewritten.
class ImageRegion final : public Core::Drawable {
public:
    ImageRegion(const std::string& path, int x, int y, int width, int height);
    glm::vec2 GetSize() const override { return m_Size; }
    void Draw(const Core::Matrices& data) override;
private:
    glm::vec2 m_Size;
    Core::Program m_Program;
    Core::VertexArray m_Quad;
    Core::UniformBuffer<Core::Matrices> m_Matrices;
    std::unique_ptr<Core::Texture> m_Texture;
};
