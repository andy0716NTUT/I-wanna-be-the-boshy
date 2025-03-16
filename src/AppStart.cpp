//
// Created by andyl on 2025/3/8.
//

#include "App.hpp"

void App::Start() {
    LOG_TRACE("Start");



    m_MenuSystem = std::make_shared<Menu>();
    m_Root.AddChild(m_MenuSystem);


    m_BGM.SetVolume(5);
    m_BGM.Play();


    std::unordered_map<Character::MoveState, std::vector<std::string>> animationPaths = {
        {Character::MoveState::IDLE, {RESOURCE_DIR"/Image/Character/idle/idle1.png"
                                                ,RESOURCE_DIR"/Image/Character/idle/idle2.png"}},
        {Character::MoveState::IDLE_LEFT, {RESOURCE_DIR"/Image/Character/idle/idle1-left.png"
            ,                                       RESOURCE_DIR"/Image/Character/idle/idle2-left.png"}},
        {Character::MoveState::JUMP, {RESOURCE_DIR"/Image/Character/jump/jump1.png",
                                                    RESOURCE_DIR"/Image/Character/jump/jump2.png",
                                                    RESOURCE_DIR"/Image/Character/jump/jump3.png"}},
        {Character::MoveState::JUMP_LEFT, {RESOURCE_DIR"/Image/Character/jump/jump1-left.png",
                                                    RESOURCE_DIR"/Image/Character/jump/jump2-left.png",
                                                    RESOURCE_DIR"/Image/Character/jump/jump3-left.png"}},
        {Character::MoveState::RUN,   {
            RESOURCE_DIR"/Image/Character/run/run1.png",
            RESOURCE_DIR"/Image/Character/run/run2.png",
            RESOURCE_DIR"/Image/Character/run/run3.png",
            RESOURCE_DIR"/Image/Character/run/run4.png",
            RESOURCE_DIR"/Image/Character/run/run5.png",
            RESOURCE_DIR"/Image/Character/run/run6.png",
            RESOURCE_DIR"/Image/Character/run/run7.png",
            RESOURCE_DIR"/Image/Character/run/run8.png"
        }},
            {Character::MoveState::RUN_LEFT,   {
                RESOURCE_DIR"/Image/Character/run/run1-left.png",
                RESOURCE_DIR"/Image/Character/run/run2-left.png",
                RESOURCE_DIR"/Image/Character/run/run3-left.png",
                RESOURCE_DIR"/Image/Character/run/run4-left.png",
                RESOURCE_DIR"/Image/Character/run/run5-left.png",
                RESOURCE_DIR"/Image/Character/run/run6-left.png",
                RESOURCE_DIR"/Image/Character/run/run7-left.png",
                RESOURCE_DIR"/Image/Character/run/run8-left.png"
            }},
        {Character::MoveState::SHOOT,{RESOURCE_DIR"/Image/Character/shoot/shoot1.png", RESOURCE_DIR"/Image/Character/shoot/shoot2.png"}},
        {Character::MoveState::SHOOT_LEFT,{RESOURCE_DIR"/Image/Character/shoot/shoot1-left.png", RESOURCE_DIR"/Image/Character/shoot/shoot2-left.png"}}
            };

    m_Boshy = std::make_shared<AnimatedCharacter>(animationPaths);
    m_Boshy->SetPosition({0, 0});
    m_Boshy->SetZIndex(-1);
    m_Root.AddChild(m_Boshy);


    m_Bullet = std::make_shared<Bullet>();
    m_Bullet->SetVisible(false);
    m_Bullet->SetZIndex(-2);
    m_Root.AddChild(m_Bullet);

    m_MapLoader = std::make_shared<MapInfoLoader>(RESOURCE_DIR"/Image/Background/Map1.txt");
    m_CurrentState = State::UPDATE;
}
