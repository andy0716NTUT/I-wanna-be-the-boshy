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
#include "Util/BGM.hpp"
#include "AnimatedCharacter.hpp"
#include "Menu.hpp"
#include "Bullet.hpp"
#include "MapInfoLoader.hpp"
#include "World.hpp"
#include "MapObject/CheckPoint.hpp"
#include <iostream>
#include "imgui.hpp"
#include "MapObject/JumpBoost.hpp"
#include "MapObject/fallingground.hpp"
#include "MapObject/Platform.hpp"
#include "GameObjectHelper.hpp"
#include "enemy.hpp"
#include "MapObject/bird.hpp"
#include "MapObject/phase2trap.hpp"

class App {
public:
    enum class GamePhase {
        MENU,
        WORLD1,
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
        std::string phaseInfo;
        std::string switchTimerInfo;
    } m_DebugInfo;

    std::vector<std::shared_ptr<Enemy>> m_Enemies;
    void TeleportToMap(const std::string& mapName);
    State GetCurrentState() const { return m_CurrentState; }
    GamePhase GetCurrentPhase() const {return m_GamePhase; }
    void ReloadMapObjects();
    bool IsOnTop(const glm::vec2& playerPos, const glm::vec2& objectPos, float objectWidth, float objectHeight);
    bool IsAABBOverlap(
        const glm::vec2& posA, float widthA, float heightA,
        const glm::vec2& posB, float widthB, float heightB,
        float yOffsetTop = 0.0f, float yExtraTop = 0.0f);

    std::string GamePhaseToString(App::GamePhase phase) {
        switch (phase) {
            case App::GamePhase::WORLD1: return "WORLD1";
            case App::GamePhase::WORLD2: return "WORLD2";
            case App::GamePhase::MENU: return "MENU";
            case App::GamePhase::END: return "END";
        }
        return "UNKNOWN";
    }
    void Respawn();
    void Start();

    void Update();

    void End(); // NOLINT(readability-convert-member-functions-to-static)
    float Gravity = -0.5f;
    App();
private:
    glm::vec2 currentCheckPoint = {0,0};
    std::string currentCheckPointPhase = "1";
    std::string CurrentPhase = "";
    int currentX,currentY;
    int checkPointX = 0, checkPointY = 0;
    std::shared_ptr<World> m_World;
    GamePhase m_GamePhase = GamePhase::MENU;
    Util::Renderer m_Root;
    Util::BGM m_BGM;
    State m_CurrentState = State::START;
    std::shared_ptr<MapInfoLoader> m_MapLoader;
    std::shared_ptr<AnimatedCharacter> m_Boshy;
    std::shared_ptr<Bullet> m_Bullet;
    std::shared_ptr<ResourceManager> m_PRM;
    std::shared_ptr<Menu> m_MenuSystem;
    std::vector<std::shared_ptr<Bullet>> m_Bullets;
    std::vector<std::shared_ptr<CheckPoint>> m_CheckPoints;
    std::vector<std::shared_ptr<JumpBoost>> m_jumpBoost;
    std::vector<std::shared_ptr<FallingGround>> m_FallingGround;
    std::vector<std::shared_ptr<Platform>> m_Platform;
    std::shared_ptr<phase2trap> m_phase2trap_up;
    std::shared_ptr<phase2trap> m_phase2trap_down;
    std::shared_ptr<Bird> m_phase8bird;
    bool trapCreated = false;
    float shootCooldown = 0;

};

#endif
