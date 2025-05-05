//
// Created by andyl on 2025/5/3.
//
#include "Boss1/BulletTypeA.hpp"

BulletTypeA::BulletTypeA(glm::vec2 bossPosition, glm::vec2 playerPosition) {
    glm::vec2 direction = glm::normalize(playerPosition - bossPosition);
    m_Velocity = direction * 1200.0f;

    std::vector<std::string> frames;
    for (int i = 1; i <= 4; i++) {
        frames.push_back(RESOURCE_DIR "/Image/Boss1/Boss_attack" + std::to_string(i) + ".png");
    }

    m_Animation = std::make_shared<Util::Animation>(frames, false, 150, true);
    m_Animation->Play();

    m_Drawable = m_Animation;
    m_Transform.translation = bossPosition;
}
glm::vec2 BulletTypeA::GetPosition() {
    return m_Transform.translation;
}

void BulletTypeA::Update(float deltaTime) {
    m_Transform.translation += m_Velocity * deltaTime;
}