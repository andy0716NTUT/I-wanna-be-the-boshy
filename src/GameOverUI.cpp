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
    // 首先将UI设为不可见
    this->SetVisible(false);
    
    // 彻底释放资源和绘制
    if (m_GameOverText) {
        // 设置绘制对象为nullptr
        SetDrawable(nullptr);
        // 释放图像资源
        m_GameOverText.reset();
        m_GameOverText = nullptr;
        // 强制清除内部状态
        m_Timer = 0.0f;
    }
}
void GameOverUI::Show() {
    // 如果资源已释放，需要重新创建
    if (!m_GameOverText) {
        m_GameOverText = std::make_shared<Util::Image>(RESOURCE_DIR "/Image/dead.png");
        SetDrawable(m_GameOverText);
    }
    
    this->SetVisible(true);
    m_Timer = 0.0f; // 重置計時器
}
void GameOverUI::Update(float deltaTime) {
    m_Timer += deltaTime;

    // 呼吸動畫：以 sin 波來改變 scale
    float scale = 1.0f + 0.05f * sin(m_Timer * 3.0f);  // 振幅 0.05，頻率 3Hz
    m_Transform.scale = {scale, scale};
}


