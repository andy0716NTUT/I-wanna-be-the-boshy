//
// Created by andyl on 2025/5/5.
//
#include "Boss1/BulletTypeC.hpp"

BulletTypeC::BulletTypeC(glm::vec2 bossPosition,glm::vec2 direction) {
    std::vector<std::string> frames;
    for (int i = 1; i <= 4; i++) {
        frames.push_back(RESOURCE_DIR "/Image/Boss1/Boss_attack" + std::to_string(i) + ".png");
    }

    m_Animation = std::make_shared<Util::Animation>(frames, true, 150, true);
    this->SetDrawable(m_Animation);
    m_Transform.translation = bossPosition;
    m_Direction = glm::normalize(direction);
}
glm::vec2 BulletTypeC::GetPosition() {
    return m_Transform.translation;
}

void BulletTypeC::Update(float deltaTime) {
    m_Transform.translation += m_Direction * m_Speed * deltaTime;
}

