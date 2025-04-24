//
// Created by andyl on 2025/3/8.
//

#include "App.hpp"

void App::Start() {
    LOG_TRACE("Start");
    currentY = 0 ;
    currentX = 0 ;
    m_World = std::make_shared<World>();
    m_BGM.SetVolume(5);
    m_BGM.Play();
    // 初始化 SDL 和 OpenGL（假設這些代碼已經存在）

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

        currentCheckPoint = m_Boshy->GetPosition();


        m_Bullet = std::make_shared<Bullet>();
        m_Bullet->SetVisible(false);
        m_Bullet->SetZIndex(-2);
        m_Root.AddChild(m_Bullet);


        std::string currentPhase = GamePhaseToString(m_GamePhase);
        auto& currentWorld = m_World->GetWorldByPhaseName(currentPhase);
        auto [currentX, currentY] = m_World->GetStartPosition(currentWorld, "1");
        World::Direction dir ;


        m_MapLoader = std::make_shared<MapInfoLoader>();
        m_MapLoader->LoadMap(currentWorld[currentX][currentY]);

        m_CheckPoints = CheckPoint::CreateFromMap(m_MapLoader, m_Root);
        m_jumpBoost = JumpBoost::CreateFromMap(m_MapLoader,m_Root);
        m_FallingGround = FallingGround::CreateFromMap(m_MapLoader,m_Root);
        m_Platform = Platform::CreateFromMap(m_MapLoader,m_Root);

        m_PRM = std::make_shared<ResourceManager>();
        m_Root.AddChild(m_PRM->GetChildren());
        m_PRM->SetPhase(currentWorld[currentX][currentY]);

        std::cout << "Test 1" << std::endl ;
        m_CurrentState = State::UPDATE;
    }
