#ifndef APP_HPP
#define APP_HPP
#include "BackgroundImage.hpp"
#include "Util/Renderer.hpp"
#include "ResourceManager.hpp"
#include "pch.hpp" // IWYU pragma: export
#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Logger.hpp"
#include "Character.hpp"
#include "AnimatedCharacter.hpp"
#include "Menu.hpp"
#include "Bullet.hpp"
#include "MapInfoLoader.hpp"
#include "World.hpp"
#include "BgmManager.hpp"
#include "MapObject/CheckPoint.hpp"
#include <iostream>

#include "Boss1/Boss1.hpp"
#include "imgui.hpp"
#include "MapObject/JumpBoost.hpp"
#include "MapObject/fallingground.hpp"
#include "MapObject/Platform.hpp"
#include "GameObjectHelper.hpp"
#include "bear.hpp"
#include "MapObject/bird.hpp"
#include "MapObject/spider.hpp"
#include "MapObject/phase2trap.hpp"
#include "GameOverUI.hpp"

class App {
public:
    enum class GamePhase {
        MENU,
        WORLD1,
        BOSS1,
        WORLD2,
        WORLD3,
        END
    };

    enum class State {
        START,
        UPDATE,
        END,
    };

    struct DebugInfo {
        std::string positionInfo;
        std::string tileInfo;
        int currentX;
        int currentY;
        std::string phaseInfo;
        std::string switchTimerInfo;
        std::string mousePtsdPos;  // PTSD座標系統中的鼠標位置
        std::string mouseGamePos;  // 遊戲座標系統中的鼠標位置
        std::string boshyPtsdPos;  // PTSD座標系統中的角色位置
        std::string overlapInfo;   // 重疊狀態信息
    } m_DebugInfo;

    void TeleportToMap(const std::string& mapName, const std::string& worldName);

    State GetCurrentState() const { return m_CurrentState; }

    GamePhase GetCurrentPhase() const {return m_GamePhase; }

    void ReloadMapObjects();
    bool GodMode = false; // 上帝模式開關
    bool IsOnTop(const glm::vec2& playerPos, const glm::vec2& objectPos, float objectWidth, float objectHeight);

    bool IsAABBOverlap(
        const glm::vec2& posA, float widthA, float heightA,
        const glm::vec2& posB, float widthB, float heightB,
        float yOffsetTop = 0.0f, float yExtraTop = 0.0f);

    std::string GamePhaseToString(App::GamePhase phase) {
        switch (phase) {
            case App::GamePhase::MENU: return "MENU";
            case App::GamePhase::WORLD1: return "WORLD1";
            case App::GamePhase::BOSS1: return "BOSS1";
            case App::GamePhase::WORLD2: return "WORLD2";
            case App::GamePhase::WORLD3: return "WORLD3";
            case App::GamePhase::END: return "END";
        }
        return "UNKNOWN";
    }
    GamePhase StringToGamePhase(const std::string& str) {
    if (str == "MENU") return GamePhase::MENU;
    if (str == "WORLD1") return GamePhase::WORLD1;
    if (str == "WORLD2") return GamePhase::WORLD2;
    if (str == "WORLD3") return GamePhase::WORLD3;
    // 依照你的 GamePhase 定義補齊
    return GamePhase::MENU; // 預設值
}
    void Respawn();
    void Start();

    void Update();

    void End(); // NOLINT(readability-convert-member-functions-to-static)
    float Gravity = -0.5f;
    App();

    std::string CurrentWorld = "";  // 移到 public 區塊
private:
    #include "AppMembers.hpp"


};

#endif
