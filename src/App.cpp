#include "App.hpp"


App::App()
    : m_BGM(RESOURCE_DIR "/BGM/FirstWorld.mp3")
{
}


void App::End() { // NOLINT(this method will mutate members in the future)
    LOG_TRACE("End");
}
void App::Respawn() {
    if (CurrentPhase != currentCheckPointPhase) {
        m_PRM->SetPhase(currentCheckPointPhase);
        m_MapLoader->LoadMap(currentCheckPointPhase);
        CurrentPhase = currentCheckPointPhase;
    }

    // 先从渲染器中移除旧对象
    for (auto& checkpoint : m_CheckPoints) {
        checkpoint->SetDrawable(nullptr);
    }
    for (auto& jumpboost : m_jumpBoost) {
        jumpboost->SetDrawable(nullptr);
    }

    // 清空容器
    m_CheckPoints.clear();
    m_jumpBoost.clear();

    // 创建新对象
    m_CheckPoints = CheckPoint::CreateFromMap(m_MapLoader, m_Root);
    m_jumpBoost = JumpBoost::CreateFromMap(m_MapLoader, m_Root);

    m_Boshy->SetPosition(currentCheckPoint);
}