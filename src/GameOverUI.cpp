//
// Created by andyl on 2025/6/12.
//
#include "GameOverUI.hpp"

GameOverUI::GameOverUI() {
    m_GameOverText = std::make_shared<Util::Image>(RESOURCE_DIR "/Image/dead.png");
    this->SetPosition({0,0});
    SetDrawable(m_GameOverText);
    this->SetZIndex(100);
    this->SetVisible(false);
}
void GameOverUI::SetPosition(const glm::vec2 &position) {
    this->m_Transform.translation = position;
}

void GameOverUI::Hide() {
    this->SetVisible(false);
}
void GameOverUI::Show() {
    this->SetVisible(true);
    m_Timer = 0.0f; // 重置計時器
}
void GameOverUI::Update(float deltaTime) {
    m_Timer += deltaTime;

    // 呼吸動畫：以 sin 波來改變 scale
    float scale = 1.0f + 0.05f * sin(m_Timer * 3.0f);  // 振幅 0.05，頻率 3Hz
    m_Transform.scale = {scale, scale};
}


