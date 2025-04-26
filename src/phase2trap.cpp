#include "MapObject/phase2trap.hpp"

#include <map>

#include "Util/Renderer.hpp"

phase2trap::phase2trap() {}

void phase2trap::SetImage(const std::string& Imagepath) {
    m_Drawable = std::make_shared<Util::Image>(Imagepath);
}

void phase2trap::SetPosition(glm::vec2 position) {
    m_Transform.translation = position;
    m_InitialY = position.y;
}

void phase2trap::SetActive(bool active) {
    isActivate = active;
}

void phase2trap::Create(std::shared_ptr<MapInfoLoader> mapLoader, const std::string& imagePath, glm::vec2 position, const std::string& movingWay, float moveRange) {
    m_Image = std::make_shared<Util::Image>(imagePath);
    m_Drawable = m_Image;
    m_Transform.translation = position;
    m_InitialY = position.y;
    MovingWay = movingWay;
    isActivate = true;
    m_MoveRange = moveRange;
    m_MapLoader = mapLoader;
}
void phase2trap::Update(float deltaTime) {
    if (!isActivate) return;

    for (const auto& tile : m_LastOccupiedTiles) {
        m_MapLoader->SetTile(tile.first, tile.second, 0);
    }
    m_LastOccupiedTiles.clear();

    if (m_IsWaiting) {
        m_WaitTimer -= deltaTime;
        if (m_WaitTimer <= 0.0f) {
            m_IsWaiting = false;
        }
        return;
    }

    const float speed = 250.0f;
    float moveDistance = speed * deltaTime;

    if (MovingWay == "down") {
        m_Transform.translation.y -= moveDistance;
        if (m_Transform.translation.y <= m_InitialY - m_MoveRange) {
            m_Transform.translation.y = m_InitialY - m_MoveRange;
            MovingWay = "up";
            m_IsWaiting = true;
            m_WaitTimer = 0.2f;
        }
    } else if (MovingWay == "up") {
        m_Transform.translation.y += moveDistance;
        if (m_Transform.translation.y >= m_InitialY + m_MoveRange) {
            m_Transform.translation.y = m_InitialY + m_MoveRange;
            MovingWay = "down";
            m_IsWaiting = true;
            m_WaitTimer = 0.2f;
        }
    }

    glm::vec2 pos = m_Transform.translation;
    int left = static_cast<int>(pos.x) / 16;
    int top = static_cast<int>(pos.y) / 16;
    int width = 64 / 16; // 假設你的trap是64x64大小
    int height = 64 / 16;

    for (int y = top; y < top + height; ++y) {
        for (int x = left; x < left + width; ++x) {
            m_MapLoader->SetTile(x, y, 5);
            m_LastOccupiedTiles.emplace_back(x, y);
        }
    }

    m_Transform.translation.x = std::round(m_Transform.translation.x);
    m_Transform.translation.y = std::round(m_Transform.translation.y);
}
void phase2trap::clear() {
     m_Drawable = nullptr;
    m_Image = nullptr;
    isActivate = false;
}

