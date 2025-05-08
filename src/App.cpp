#include "App.hpp"


App::App(){};


void App::End() { // NOLINT(this method will mutate members in the future)
    LOG_TRACE("End");
}
void App::Respawn() {
    isDead = true;
    NotFirstTryBoss = true;
    currentX = checkPointX;
    currentY = checkPointY;
    if (CurrentPhase != currentCheckPointPhase) {
        m_PRM->SetPhase(currentCheckPointPhase,CurrentWorld);
        m_MapLoader->LoadMap(currentCheckPointPhase,CurrentWorld);
        CurrentPhase = currentCheckPointPhase;
    }

    // 清除所有游戏对象
    ReloadMapObjects();

    m_Boshy->SetPosition(currentCheckPoint);

    isDead = false;
}


void App::ReloadMapObjects() {
    ClearGameObjects(m_Platform);
    ClearGameObjects(m_CheckPoints);
    ClearGameObjects(m_jumpBoost);
    ClearGameObjects(m_FallingGround);
    ClearGameObjects(m_Enemies);
    if ((isDead && m_phase8bird) || (m_phase8bird && !(CurrentPhase == "8" || CurrentPhase == "9" || CurrentPhase == "10" || CurrentPhase == "11" || CurrentPhase == "12")))ClearGameObjects(m_phase8bird);

    m_CheckPoints.clear();
    m_jumpBoost.clear();
    m_Platform.clear();
    m_FallingGround.clear();
    m_Enemies.clear();  // 清空敌人容器

    m_CheckPoints = CheckPoint::CreateFromMap(m_MapLoader, m_Root);
    m_jumpBoost = JumpBoost::CreateFromMap(m_MapLoader, m_Root);
    m_FallingGround = FallingGround::CreateFromMap(m_MapLoader, m_Root);
    m_Platform = Platform::CreateFromMap(m_MapLoader, m_Root);
    m_Enemies = Enemy::CreateFromMap(m_MapLoader, m_Root);  // 重新创建敌人
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
void App::TeleportToMap(const std::string& mapName) {
    // 保存地图名称
    CurrentPhase = mapName;

    // 清除所有子弹
    ClearGameObjects(m_Bullets);
    m_Bullets.clear();
    // 清除现有游戏对象
    ClearGameObjects(m_Platform);
    ClearGameObjects(m_FallingGround);
    ClearGameObjects(m_Enemies);
    ClearGameObjects(m_CheckPoints);
    ClearGameObjects(m_jumpBoost);
    m_CheckPoints.clear();
    m_jumpBoost.clear();

    // 设置资源管理器阶段并加载新地图
    m_PRM->SetPhase(mapName,CurrentWorld);
    m_MapLoader->LoadMap(mapName,CurrentWorld);


    // 重新创建游戏对象
    m_CheckPoints = CheckPoint::CreateFromMap(m_MapLoader, m_Root);
    m_jumpBoost = JumpBoost::CreateFromMap(m_MapLoader, m_Root);
    m_FallingGround = FallingGround::CreateFromMap(m_MapLoader, m_Root);
    m_Platform = Platform::CreateFromMap(m_MapLoader, m_Root);
    m_Enemies = Enemy::CreateFromMap(m_MapLoader, m_Root);

    // 重置玩家位置到合适的起始点
    glm::vec2 startPos = {0.0f, 0.0f};

    // 根据地图设置不同的起始位置
    if (mapName == "1") {
        startPos = {-500.0f, 0.0f};
    } else if (mapName == "2") {
        startPos = {-500.0f, 100.0f};
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
        currentX = 2;
        currentY = 5;
    }

    // 设置角色位置
    m_Boshy->SetPosition(startPos);

    // 更新检查点位置
    currentCheckPoint = startPos;
    currentCheckPointPhase = mapName;

    std::cout << "已传送到地图: " << mapName << std::endl;
}