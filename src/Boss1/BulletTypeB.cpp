//
// Created by andyl on 2025/5/4.
//
#include "Boss1/BulletTypeB.hpp"


BulletTypeB::BulletTypeB(glm::vec2 position) {
    m_Image = std::make_shared<Util::Image>(RESOURCE_DIR"/Image/Boss1/fallingAttack.png");
    this->SetDrawable(m_Image);
    m_StartX = position.x;
    m_StartY = position.y;
    m_TargetX = m_StartX;
    m_TargetY = m_StartY - 40.0f;
    m_BounceY = m_StartY - 60.0f; // 下墜過頭 → 彈上來
    m_Transform.translation = position;
}
glm::vec2 BulletTypeB::GetPosition() {
    return this->m_Transform.translation;
}

void BulletTypeB::Update(float deltaTime) {
    m_AnimTimer += deltaTime;
    float t = m_AnimTimer / m_AnimDuration;
    if (t > 1.0f) t = 1.0f;

    float currentY;

    if (t < 0.6f) {
        // ⭐ 第 1 段：下墜 → 到 m_BounceY
        float localT = t / 0.5f;  // 0~1
        currentY = m_StartY + (m_BounceY - m_StartY) * (localT * localT);  // ease-in 下墜
    } else {
        // ⭐ 第 2 段：從 m_BounceY 回到 m_TargetY
        float localT = (t - 0.5f) / 0.5f;  // 0~1
        float easeT = 1 - (1 - localT) * (1 - localT);  // ease-out 回落
        currentY = m_BounceY + (m_TargetY - m_BounceY) * easeT;
    }

    // X 緩慢移動
    float currentX = m_StartX + (m_TargetX - m_StartX) * t;

    m_Transform.translation.x = currentX;
    m_Transform.translation.y = currentY;

    // ⭐ 當一格動畫完成
    if (m_AnimTimer >= m_AnimDuration) {
        m_AnimTimer = 0.0f;

        // 更新下一段起點 / 目標
        m_StartY = m_TargetY;
        m_TargetY -= 40.0f;
        m_BounceY = m_StartY - 60.0f;  // 每次多下墜 5 單位過頭 → 回彈

        m_StartX = m_TargetX;
        int randNum = rand() % 3;  // 0:左 1:不動 2:右
        if (randNum == 0) m_TargetX = m_StartX - 30.0f;
        else if (randNum == 1) m_TargetX = m_StartX;
        else m_TargetX = m_StartX + 30.0f;
    }
}




