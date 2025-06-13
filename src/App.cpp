#include "App.hpp"


App::App() {
    currentCheckPoint = {0,0};
    currentCheckPointPhase = "1";
    CurrentPhase = "";
    checkPointX = 0;
    checkPointY = 0;
    checkPointWorld = "WORLD1";
    m_CurrentState = State::START;
    m_GamePhase = GamePhase::MENU;

    trapCreated = false;
    deathType = DeathType::NONE;  // 替换原来的 isDead = false
    shootCooldown = 0;
    NotFirstTryBoss= false;
};


void App::End() { // NOLINT(this method will mutate members in the future)
    LOG_TRACE("End");
}
void App::Respawn() {
    // 确保死亡UI被彻底隐藏和清除
    m_GameOverUI->Hide();
    
    // 恢复游戏状态
    NotFirstTryBoss = true;
    currentX = checkPointX;
    currentY = checkPointY;
    CurrentWorld = checkPointWorld;
    m_GamePhase = StringToGamePhase(CurrentWorld);
    if (CurrentPhase != currentCheckPointPhase) {
        m_PRM->SetPhase(currentCheckPointPhase,CurrentWorld);
        m_MapLoader->LoadMap(currentCheckPointPhase,CurrentWorld);
        CurrentPhase = currentCheckPointPhase;
    }
    m_PRM->resetRotation();
    
    // 清除所有游戏对象
    ReloadMapObjects();

    // 重置角色位置
    m_Boshy->SetPosition(currentCheckPoint);

    // 根據世界切換BGM
    if (m_GamePhase == GamePhase::WORLD1) m_BGM->SetBGM("WORLD1");
    else if (m_GamePhase == GamePhase::WORLD2) m_BGM->SetBGM("WORLD2");
    else if (m_GamePhase == GamePhase::BOSS1) m_BGM->SetBGM("Boss1");

    // 重置死亡状态
    deathType = DeathType::NONE;
    deathAnimFinished = false;
    deathAnimTimer = 0.0f;
    
    // 强制更新绘制，确保死亡UI不会被渲染
    m_Root.Update();
}


void App::ReloadMapObjects() {    
    ClearGameObjects(m_Platform);
    ClearGameObjects(m_CheckPoints);
    ClearGameObjects(m_CheckpointBullets, m_Root); // 清除檢查點子彈並從渲染器移除
    ClearGameObjects(m_jumpBoost);
    ClearGameObjects(m_FallingGround);
    if ((deathType != DeathType::NONE && m_phase8bird) || (m_phase8bird && !(CurrentPhase == "8" || CurrentPhase == "9" || CurrentPhase == "10" || CurrentPhase == "11" || CurrentPhase == "12")))
        ClearGameObjects(m_phase8bird);
    if (GamePhaseToString(m_GamePhase) != "WORLD1" && m_bear && !(CurrentPhase.find("4_") == 0 || CurrentPhase == "5")) {
        ClearGameObjects(m_bear);
    }
    if (m_phase8spider) {
        m_phase8spider->RestoreAllTiles();
        ClearGameObjects(m_phase8spider);
    }
    if (m_Boss1) {
        m_Boss1->ClearAnimation(m_Root);
        ClearGameObjects(m_Boss1);
    }    
    m_CheckPoints.clear();
    m_CheckpointBullets.clear(); // 清除檢查點子彈列表
    m_jumpBoost.clear();
    m_Platform.clear();
    m_FallingGround.clear();

    m_CheckPoints = CheckPoint::CreateFromMap(m_MapLoader, m_Root);
    m_jumpBoost = JumpBoost::CreateFromMap(m_MapLoader, m_Root);
    m_FallingGround = FallingGround::CreateFromMap(m_MapLoader, m_Root);
    m_Platform = Platform::CreateFromMap(m_MapLoader, m_Root);
}
bool App::IsOnTop(const glm::vec2& playerPos, const glm::vec2& objectPos, float objectWidth, float objectHeight) {
    float fgTop = objectPos.y + objectHeight / 2;
    float playerBottom = playerPos.y - 8; // playerHeight / 2
    float playerLeft = playerPos.x - 8;
    float playerRight = playerPos.x + 8;
    float fgLeft = objectPos.x - objectWidth / 2;
    float fgRight = objectPos.x + objectWidth / 2;

    return playerBottom >= fgTop - 4 && playerBottom <= fgTop + 4 &&
           playerRight > fgLeft && playerLeft < fgRight;
}

bool App::IsAABBOverlap(
    const glm::vec2& posA, float widthA, float heightA,
    const glm::vec2& posB, float widthB, float heightB,
    float yOffsetTop, float yExtraTop) {

    float leftA = posA.x - widthA / 2;
    float rightA = posA.x + widthA / 2;
    float topA = posA.y + heightA / 2;
    float bottomA = posA.y - heightA / 2;

    float leftB = posB.x - widthB / 2;
    float rightB = posB.x + widthB / 2;
    float topB = posB.y + heightB / 2;
    float bottomB = posB.y - heightB / 2;

    return rightA > leftB && leftA < rightB &&
           bottomA < topB && (topA + yOffsetTop) > bottomB - yExtraTop;
}
void App::TeleportToMap(const std::string& mapName, const std::string& worldName) {
    // 保存地图名称和世界名称
    CurrentPhase = mapName;
    CurrentWorld = worldName;

    // 更新遊戲階段
    if (worldName == "WORLD1") {
        m_GamePhase = GamePhase::WORLD1;
    } else if (worldName == "WORLD2") {
        m_GamePhase = GamePhase::WORLD2;
    } else if (worldName == "WORLD3") {
        m_GamePhase = GamePhase::WORLD3;
    }    // 清除所有子弹
    ClearGameObjects(m_Bullets);
    m_Bullets.clear();
    // 清除檢查點子彈
    ClearGameObjects(m_CheckpointBullets, m_Root);
    m_CheckpointBullets.clear();
    // 清除现有游戏对象    ClearGameObjects(m_Platform);

    // 设置资源管理器阶段并加载新地图
    m_PRM->SetPhase(mapName, CurrentWorld);
    m_MapLoader->LoadMap(mapName, CurrentWorld);

    ReloadMapObjects();
    // 更新世界地圖位置
    auto& currentWorld = m_World->GetWorldByPhaseName(CurrentWorld);
    std::tie(currentX, currentY) = m_World->GetStartPosition(currentWorld, mapName);

    // 根據地圖設置起始位置
    glm::vec2 startPos = {0.0f, 0.0f};
    if (mapName == "1") {
        startPos = {-500.0f, 0.0f};
    } else if (mapName == "2") {
        startPos = {-610.0f, -372.0f};
    } else if (mapName.find("3") == 0) {
        startPos = {-500.0f, 200.0f};
    } else if (mapName.find("4") == 0) {
        startPos = {-500.0f, 100.0f};
    } else if (mapName == "5") {
        startPos = {-500.0f, 100.0f};
    } else if (mapName.find("6") == 0) {
        startPos = {320.0f, 200.0f};
    } else if (mapName.find("7") == 0) {
        startPos = {600.0f, 100.0f};
    } else if (mapName == "8") {
        startPos = {-500.0f, 100.0f};
    } else if (mapName.find("9") == 0) {
        startPos = {-500.0f, 200.0f};
    } else if (mapName.find("10") == 0) {
        startPos = {-500.0f, 100.0f};
    } else if (mapName.find("11") == 0) {
        startPos = {-500.0f, 200.0f};
    } else if (mapName.find("12") == 0) {
        startPos = {525.0f, 180.0f};
    }    // 设置角色位置
    m_Boshy->SetPosition(startPos);
    std::tie(currentY, currentX) = m_World->GetPhaseIndex(mapName, CurrentWorld);
    
    // 更新检查点位置
    currentCheckPoint = startPos;
    currentCheckPointPhase = mapName;
    checkPointX = currentX;
    checkPointY = currentY;
    checkPointWorld = worldName;
    
    if(worldName == "WORLD1"){
        m_GamePhase = GamePhase::WORLD1;
    }else if(worldName == "WORLD2"){
        m_GamePhase = GamePhase::WORLD2;
    }else if(worldName == "WORLD3"){
        m_GamePhase = GamePhase::WORLD3;
    }


    std::cout << "已传送到世界: " << worldName << ", 地图: " << mapName 
              << ", 位置: (" << currentX << ", " << currentY << ")" << std::endl;
}