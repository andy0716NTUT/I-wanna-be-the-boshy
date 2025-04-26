#include "MapObject/phase2trap.hpp"
#include <cmath> // 加入 sin()
phase2trap::phase2trap() {}

void phase2trap::Create(const std::string& imagePath, glm::vec2 position, float moveRange) {
    m_Image = std::make_shared<Util::Image>(imagePath);
    m_Drawable = m_Image;
    m_Transform.translation = position;
    m_InitialY = position.y;
    m_MoveRange = moveRange;
    m_Timer = 0.0f;
    isActivate = true;
}

void phase2trap::Update(float deltaTime) {
    if (!isActivate) return;
    m_LastOccupiedTiles.clear();
    m_Timer += deltaTime;
    // 使用正弦波讓他上下移動
    m_Transform.translation.y = m_InitialY + std::sin(m_Timer * 2.0f) * m_MoveRange;

    m_Transform.translation.x = std::round(m_Transform.translation.x);
    m_Transform.translation.y = std::round(m_Transform.translation.y);
}

void phase2trap::clear() {
    m_Drawable = nullptr;
    m_Image = nullptr;
    isActivate = false;
}

void phase2trap::SetActive(bool active) {
    isActivate = active;
}
