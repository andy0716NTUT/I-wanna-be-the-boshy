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

#include "App.hpp"
#include "GameObjectHelper.hpp"

void App::ReloadMapObjects() {
    ClearGameObjects(m_CheckPoints);
    ClearGameObjects(m_jumpBoost);
    ClearGameObjects(m_FallingGround);
    ClearGameObjects(m_Platform);

    m_CheckPoints = CreateGameObjectsFromMap<CheckPoint>(m_MapLoader, m_Root);
    m_jumpBoost = CreateGameObjectsFromMap<JumpBoost>(m_MapLoader, m_Root);
    m_FallingGround = CreateGameObjectsFromMap<FallingGround>(m_MapLoader, m_Root);
    m_Platform = CreateGameObjectsFromMap<Platform>(m_MapLoader, m_Root);
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
