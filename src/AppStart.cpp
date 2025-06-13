//
// Created by andyl on 2025/3/8.
//

#include "App.hpp"
#include "GameObjectHelper.hpp"
#include "spdlog/fmt/bundled/chrono.h"

void App::Start() {
    LOG_TRACE("Start");

    currentX = 0;
    currentY = 0;

    m_World = std::make_shared<World>();
    m_BGM = std::make_shared<BgmManager>(RESOURCE_DIR"/BGM/MENU.mp3");
    // 初始化角色動畫
    std::unordered_map<Character::MoveState, std::vector<std::string>> animationPaths = {
        {Character::MoveState::IDLE, {RESOURCE_DIR"/Image/Character/idle/idle1.png", RESOURCE_DIR"/Image/Character/idle/idle2.png"}},
        {Character::MoveState::IDLE_LEFT, {RESOURCE_DIR"/Image/Character/idle/idle1-left.png", RESOURCE_DIR"/Image/Character/idle/idle2-left.png"}},
        {Character::MoveState::JUMP, {RESOURCE_DIR"/Image/Character/jump/jump1.png", RESOURCE_DIR"/Image/Character/jump/jump2.png", RESOURCE_DIR"/Image/Character/jump/jump3.png"}},
        {Character::MoveState::JUMP_LEFT, {RESOURCE_DIR"/Image/Character/jump/jump1-left.png", RESOURCE_DIR"/Image/Character/jump/jump2-left.png", RESOURCE_DIR"/Image/Character/jump/jump3-left.png"}},
        {Character::MoveState::RUN, {
            RESOURCE_DIR"/Image/Character/run/run1.png", RESOURCE_DIR"/Image/Character/run/run2.png",
            RESOURCE_DIR"/Image/Character/run/run3.png", RESOURCE_DIR"/Image/Character/run/run4.png",
            RESOURCE_DIR"/Image/Character/run/run5.png", RESOURCE_DIR"/Image/Character/run/run6.png",
            RESOURCE_DIR"/Image/Character/run/run7.png", RESOURCE_DIR"/Image/Character/run/run8.png"
        }},
        {Character::MoveState::RUN_LEFT, {
            RESOURCE_DIR"/Image/Character/run/run1-left.png", RESOURCE_DIR"/Image/Character/run/run2-left.png",
            RESOURCE_DIR"/Image/Character/run/run3-left.png", RESOURCE_DIR"/Image/Character/run/run4-left.png",
            RESOURCE_DIR"/Image/Character/run/run5-left.png", RESOURCE_DIR"/Image/Character/run/run6-left.png",
            RESOURCE_DIR"/Image/Character/run/run7-left.png", RESOURCE_DIR"/Image/Character/run/run8-left.png"
        }},
        {Character::MoveState::SHOOT, {RESOURCE_DIR"/Image/Character/shoot/shoot1.png", RESOURCE_DIR"/Image/Character/shoot/shoot2.png"}},
        {Character::MoveState::SHOOT_LEFT, {RESOURCE_DIR"/Image/Character/shoot/shoot1-left.png", RESOURCE_DIR"/Image/Character/shoot/shoot2-left.png"}},
        // 添加死亡動畫
        {Character::MoveState::DEATH, {
            RESOURCE_DIR"/Image/Character/death/death1.png",
            RESOURCE_DIR"/Image/Character/death/death2.png",
            RESOURCE_DIR"/Image/Character/death/death3.png",
            RESOURCE_DIR"/Image/Character/death/death4.png"
        }},
        {Character::MoveState::DEATH_LEFT, {
            RESOURCE_DIR"/Image/Character/death/death1-left.png",
            RESOURCE_DIR"/Image/Character/death/death2-left.png",
            RESOURCE_DIR"/Image/Character/death/death3-left.png",
            RESOURCE_DIR"/Image/Character/death/death4-left.png"
        }}
    };

    m_Boshy = std::make_shared<AnimatedCharacter>(animationPaths);

    m_Boshy->SetZIndex(-1);    currentCheckPoint = m_Boshy->GetPosition();    
    m_Bullet = std::make_shared<Bullet>();
    m_Bullet->SetVisible(false);
    m_Bullet->SetZIndex(-2);
    
    // 初始化檢查點子彈向量
    m_CheckpointBullets.clear();// Initialize bear
    m_bear = std::make_shared<bear>();
    m_bear->SetZIndex(-2);
    m_bear->SetVisible(false); // Initially invisible until detected in phase 4
    m_Root.AddChild(m_bear); // Add to renderer right away
    m_Root.AddChild(m_bear);

    m_MapLoader = std::make_shared<MapInfoLoader>();

    m_PRM = std::make_shared<ResourceManager>();

    m_Menu = std::make_shared<Menu>();
    m_Menu->SetImage(RESOURCE_DIR"/Image/Menu/menu_game.png");

    m_Root.AddChild(m_Menu);
    m_GameOverUI = std::make_shared<GameOverUI>();
    m_GameOverUI->Hide();

    m_Root.AddChild(m_GameOverUI);
    m_Root.Update();
    m_CurrentState = State::UPDATE;
}
