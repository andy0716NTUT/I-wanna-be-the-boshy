//
// Created by andyl on 2025/3/8.
//

#include "App.hpp"

void App::Start() {
    LOG_TRACE("Start");

    m_BGM.SetVolume(5);
    m_BGM.Play();


    m_Boshy->SetPosition({-608, -128});
    m_Boshy->SetZIndex(-1);
    m_Root.AddChild(m_Boshy);



    m_PRM = std::make_shared<ResourceManager>();
    m_Root.AddChild(m_PRM->GetChildren());
    m_CurrentState = State::UPDATE;
}