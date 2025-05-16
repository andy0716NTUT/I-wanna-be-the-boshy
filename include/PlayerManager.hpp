#ifndef PLAYER_MANAGER_HPP
#define PLAYER_MANAGER_HPP

#include "glm/glm.hpp"
#include <vector>
#include <memory> // For std::shared_ptr

// Forward declarations
class AnimatedCharacter;
class MapInfoLoader;
class Bullet;
class JumpBoost;
namespace Util {
    class Renderer;
    // Util::Keycode is usually in its own header or part of Input.hpp
    // Assuming it's accessible or Input.hpp will be included in PlayerManager.cpp
}
class App; // To access App members like Gravity constants, Respawn(), etc.

class PlayerManager {
public:
    PlayerManager(AnimatedCharacter* boshy,
                  MapInfoLoader* mapLoader,
                  std::vector<std::shared_ptr<Bullet>>* bullets_vector_ptr,
                  std::vector<std::shared_ptr<JumpBoost>>* jumpBoost_vector_ptr,
                  Util::Renderer* root_renderer_ptr,
                  App* app_instance);

    void Update(float deltaTime,
                bool godMode,
                // Tile information around the player, calculated in App::Update
                int playerTileX,
                int playerTileY,
                int tileAbovePlayer,
                int tileBelowPlayer,
                int tileLeftOfPlayer,
                int tileRightOfPlayer,
                // Player's position, modified by this manager
                glm::vec2& playerPosition);

    int GetJumpCount() const { return m_jumpCount; }

private:
    AnimatedCharacter* m_boshy_ptr;
    MapInfoLoader* m_mapLoader_ptr;
    std::vector<std::shared_ptr<Bullet>>* m_bullets_vector_ptr;
    std::vector<std::shared_ptr<JumpBoost>>* m_jumpBoost_vector_ptr;
    Util::Renderer* m_root_renderer_ptr;
    App* m_app_ptr; // Pointer to the main App class instance

    // Player state variables, formerly static in App::Update
    float m_velocityY;
    int m_jumpCount;
    bool m_isJumping;
    float m_shootCooldown;
    // Note: deltaTime is passed in Update, not stored as member.
    // Constants like Gravity, JumpPower will be accessed via m_app_ptr.
};

#endif // PLAYER_MANAGER_HPP 