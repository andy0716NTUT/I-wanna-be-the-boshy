//
// Created by andyl on 2025/5/4.
//
#include "Boss1/BulletTypeB.hpp"


BulletTypeB::BulletTypeB(glm::vec2 position) {
    m_Image = std::make_shared<Util::Image>(RESOURCE_DIR"/Image/Boss1/fallingAttack.png");
    this->SetDrawable(m_Image);
    m_Transform.translation = position;
}
glm::vec2 BulletTypeB::GetPosition() {
    return this->m_Transform.translation;
}

void BulletTypeB::Update(float deltaTime) {
    m_MoveTimer += deltaTime;

    if (m_MoveTimer >= 0.3f) {  // ⭐ 每 0.3 秒更新一次
        m_MoveTimer = 0.0f;

        // ⭐ 一定往下
        m_Transform.translation.y -= 30.0f;

        // ⭐ 隨機 X 偏移
        int randNum = rand() % 3;  // 0: 左  1: 不動  2: 右
        if (randNum == 0) {
            m_Transform.translation.x -= 30.0f;
        } else if (randNum == 2) {
            m_Transform.translation.x += 30.0f;
        }
        // randNum == 1 → 不動
    }
}

