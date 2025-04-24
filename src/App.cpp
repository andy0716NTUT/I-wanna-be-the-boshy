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
    for (auto& checkpoint : m_CheckPoints) {
        checkpoint->SetDrawable(nullptr);
    }
    m_CheckPoints = CheckPoint::CreateFromMap(m_MapLoader, m_Root);
    m_jumpBoost = JumpBoost::CreateFromMap(m_MapLoader,m_Root);
    m_CheckPoints.clear();
    m_jumpBoost.clear();
    m_Boshy->SetPosition(currentCheckPoint);
}