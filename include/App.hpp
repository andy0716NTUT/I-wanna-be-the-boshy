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
#include <iostream>
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
    State GetCurrentState() const { return m_CurrentState; }

    void Start();

    void Update();

    void End(); // NOLINT(readability-convert-member-functions-to-static)

    App();
private:
    GamePhase m_GamePhase = GamePhase::MENU;
    Util::Renderer m_Root;
    Util::BGM m_BGM;
    State m_CurrentState = State::START;
    std::shared_ptr<MapInfoLoader> m_MapLoader;
    std::shared_ptr<AnimatedCharacter> m_Boshy;
    std::shared_ptr<Bullet> m_Bullet;
    std::shared_ptr<ResourceManager> m_PRM;
    std::shared_ptr<Menu> m_MenuSystem;
};

#endif
