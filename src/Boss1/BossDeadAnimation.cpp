//
// Created by andyl on 2025/5/7.
//

#include "Boss1/BossDeadAnimation.hpp"


BossDeadAnimation::BossDeadAnimation() {
    std::vector<std::string> DeadFrames;
    for (int i = 1; i <= 4; i++) {
        DeadFrames.push_back(RESOURCE_DIR "/Image/Boss1/blast" + std::to_string(i) + ".png");
    }
    m_Animation = std::make_shared<Util::Animation>(DeadFrames, true, 200, true);
    SetDrawable(m_Animation);
    SetZIndex(5);
}
void BossDeadAnimation::SetPOsition(glm::vec2 position) {
    this->m_Transform.translation = position;
}

