//
// Created by andyl on 2025/5/3.
//
#include "Boss1/LightAttack.hpp"


LightAttack::LightAttack(glm::vec2 playerPosition) {
    std::vector<std::string> frame1;
    std::vector<std::string> frame2;
    for (int i = 1 ; i <= 2 ; i ++) {
        frame1.push_back(RESOURCE_DIR"/Image/Boss1/lightAttack" + std::to_string(i) + ".png");
    }
    for (int i = 3 ; i <= 4 ; i++) {
        frame2.push_back(RESOURCE_DIR"/Image/Boss1/lightAttack" + std::to_string(i) + ".png");
    }

    m_Animation_Waiting = std::make_shared<Util::Animation>(frame1,false,100,true);
    m_Animation_Shoot = std::make_shared<Util::Animation>(frame2,false,100,true);

    m_Transform.translation.y = playerPosition.y;
    SetDrawable(m_Animation_Waiting);
    m_Animation_Waiting->Play();
}

void LightAttack::Update(float deltaTime) {
    m_WaitTimer += deltaTime;

    if (!m_HasSwitched && m_WaitTimer >= 2.0f) {
        SetDrawable(m_Animation_Shoot);
        m_HasSwitched = true;
    }

    if (m_HasSwitched) {
        m_Animation_Shoot->Play();
    } else {
        m_Animation_Waiting->Play();
    }
}
