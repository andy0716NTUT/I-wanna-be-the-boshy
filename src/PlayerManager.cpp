#include "PlayerManager.hpp"
#include "Util/Input.hpp" // For Util::Input::IsKeyPressed, Util::Keycode::*
#include "Character.hpp"    // For Character::direction and Character::MoveState
#include "AnimatedCharacter.hpp" // For dynamic_cast to AnimatedCharacter
#include "Bullet.hpp"       // For Bullet::CreateBullet
#include "App.hpp"          // For App specific members like Gravity, Respawn, CurrentPhase
#include "MapInfoLoader.hpp"// For m_mapLoader_ptr methods
#include "MapObject/JumpBoost.hpp" // <<< 包含 JumpBoost 的完整定義

// Constructor
PlayerManager::PlayerManager(AnimatedCharacter* boshy,
                             MapInfoLoader* mapLoader,
                             std::vector<std::shared_ptr<Bullet>>* bullets_vector_ptr,
                             std::vector<std::shared_ptr<JumpBoost>>* jumpBoost_vector_ptr, // <<< 新增參數
                             Util::Renderer* root_renderer_ptr,
                             App* app_instance)
    : m_boshy_ptr(boshy),
      m_mapLoader_ptr(mapLoader),
      m_bullets_vector_ptr(bullets_vector_ptr),
      m_jumpBoost_vector_ptr(jumpBoost_vector_ptr), // <<< 初始化新成員
      m_root_renderer_ptr(root_renderer_ptr),
      m_app_ptr(app_instance),
      m_velocityY(0.0f),      // Initialize player states
      m_jumpCount(0),
      m_isJumping(false),
      m_shootCooldown(0.0f) {
    // Initialization, if any, beyond member initialization list
}

// PlayerManager::Update method will be implemented by moving logic from App::Update
void PlayerManager::Update(float deltaTime,
                           bool godMode,
                           int playerTileX,
                           int playerTileY,
                           int tileAbovePlayer,
                           int tileBelowPlayer,
                           int tileLeftOfPlayer,
                           int tileRightOfPlayer,
                           glm::vec2& playerPosition) {
    // Ensure m_app_ptr is valid before dereferencing
    if (!m_app_ptr || !m_boshy_ptr || !m_mapLoader_ptr || !m_bullets_vector_ptr || !m_root_renderer_ptr /* m_jumpBoost_vector_ptr can be null if no boosts */) {
        // Handle error: one of the essential pointers is null
        // This could be an assertion or logging, or simply return.
        // For now, let's assume they are always valid if initialization was correct.
        return;
    }

    // Check if the character is dead
    bool isCharacterDead = (m_app_ptr->deathType == DeathType::REAL_DEATH);

    // Access constants from App
    const float gravity = m_app_ptr->Gravity; // Assuming Gravity is a public member or has a getter
    const float jumpPower = 16.0f; // Taken from App::Update, consider making this App member too
    const float maxFallSpeed = -10.0f; // Taken from App::Update, consider making this App member too

    // Manage shoot cooldown (moved from App::Update)
    m_shootCooldown -= deltaTime * 4; // Factor 4 was in App::Update, might need adjustment or be a constant

    // Apply gravity (even when dead, to allow death animation to complete)
    m_velocityY += gravity;
    if (m_velocityY < maxFallSpeed) {
        m_velocityY = maxFallSpeed;
    }
    playerPosition.y += m_velocityY;

    // Cast to AnimatedCharacter to set states
    auto* animatedBoshy = dynamic_cast<AnimatedCharacter*>(m_boshy_ptr);
    if (!animatedBoshy) return; // Safety check

    // Head collision (includes the pass-through logic for WORLD2 phase 3)
    // This uses tileAbovePlayer, playerTileY, m_app_ptr->m_GamePhase, m_app_ptr->CurrentPhase, playerPosition.x
    // Note: CurrentPhase is a private member of App, PlayerManager needs a way to get it if App.hpp doesn't expose it.
    // For now, assuming CurrentPhase can be accessed via m_app_ptr->GetCurrentPhaseString() or similar getter.
    // Let's assume App::GetGamePhase() and App::GetCurrentPhaseName() exist.
    if ((tileAbovePlayer == 2 || tileAbovePlayer == 1) && m_velocityY > 0) {
        bool allowPassThroughSpecificPlatform = false;
        if (m_app_ptr->GetGamePhase() == App::GamePhase::WORLD2 && m_app_ptr->GetCurrentPhaseName() == "3" && tileAbovePlayer == 1) {
            int mapY_of_tile_above = playerTileY - 1;
            if (mapY_of_tile_above == 23) { // Target platform's map Y is 23 (for world Y 108)
                if (playerPosition.x >= -405.0f && playerPosition.x <= -360.0f) {
                    allowPassThroughSpecificPlatform = true;
                }
            }
        }

        if (!allowPassThroughSpecificPlatform) {
            // Original collision response: playerPosition.y = 480 - ((playerTileY + 1) * 16);
            // This calculation seems to be: top_of_screen - ( (current_player_tile_Y_idx + 1) * tile_height)
            // which means the top_edge of the tile *below* the player's current tile.
            // If head hits tile (playerTileY-1), feet (playerPosition.y) should be at bottom of (playerTileY-1)
            // Bottom of (playerTileY-1) is 480 - ( (playerTileY-1) + 1 )*16 = 480 - playerTileY * 16
            // Let's use the one from AppUpdate.cpp for now and review later if it behaves strangely.
             playerPosition.y = 480.0f - ((static_cast<float>(playerTileY) + 1.0f) * 16.0f);
            m_velocityY = 0;
        }
    }

    // Feet collision
    if ((tileBelowPlayer == 1 || tileBelowPlayer == 8) && m_velocityY < 0) {
        // playerPosition.y = 480 - ((playerTileY) * 16) - 4; // Original from AppUpdate
        // This means top_edge of player's current tile, minus 4 pixels.
        // Correct landing should be on top of tileBelowPlayer (tileY+1)
        // Top of tile (playerTileY+1) is 480 - (playerTileY+1)*16
        playerPosition.y = 480.0f - (static_cast<float>(playerTileY + 1) * 16.0f); // Land on top of the tile
        m_velocityY = 0;
        m_jumpCount = 0;
        m_isJumping = false; // Landed
    }

    // Collision if player is *inside* a solid block (value 1)
    // This was: if (m_MapLoader->GetTile(playerTileX, playerTileY) == 1 && velocityY >= 0){
    // This check seems problematic if velocityY can be 0 and player is stuck.
    // And playerPosition.y = 480 - (playerTileY - 1) * 16; (pushes to top of tile above current)
    // Let's keep it for now for functional parity.
    if (m_mapLoader_ptr->GetTile(playerTileX, playerTileY) == 1 && m_velocityY >= 0) {
        playerPosition.y = 480.0f - (static_cast<float>(playerTileY - 1) * 16.0f);
        m_velocityY = 0; // Stop vertical movement if stuck and trying to move up/is still.
    }

    // Reset jump count if not on solid ground (or allow double jump from air)
    if (!(tileBelowPlayer == 1 || tileBelowPlayer == 8)) {
        if (m_jumpCount == 0) { 
            m_jumpCount = 1;    
        }
    }

    // JumpBoost Interaction (MOVED HERE)
    if (m_jumpBoost_vector_ptr) { // Check if the pointer to the vector is valid
        for (auto& boost : *m_jumpBoost_vector_ptr) { // Dereference to get the vector
            if (boost) { // Check if the boost shared_ptr itself is valid
                boost->CheckInteraction(playerPosition, m_jumpCount); // m_jumpCount is lvalue member
            }
        }
    }

    // If character is dead, disable all movement input except for R key (which is handled in App::Update)
    // The R key logic is already implemented in App::Update to respawn the player
    if (isCharacterDead) {
        // Keep the character in death state
        if (animatedBoshy->GetDirection() == Character::direction::LEFT) {
            animatedBoshy->SetState(Character::MoveState::DEATH_LEFT);
        } else {
            animatedBoshy->SetState(Character::MoveState::DEATH);
        }
        return; // Skip all movement controls
    }

    // --- Handle Input ---
    // Horizontal movement
    if (Util::Input::IsKeyPressed(Util::Keycode::RIGHT)) {
        if (tileRightOfPlayer != 2) { // Not a wall type 2
            playerPosition.x += 5.0f;
            animatedBoshy->SetDirection(Character::direction::RIGHT);
            animatedBoshy->SetState(Character::MoveState::RUN);
        } else {
            animatedBoshy->SetState(Character::MoveState::IDLE);
        }
    } else if (Util::Input::IsKeyPressed(Util::Keycode::LEFT)) {
        if (tileLeftOfPlayer != 2) { // Not a wall type 2
            playerPosition.x -= 5.0f;
            animatedBoshy->SetDirection(Character::direction::LEFT);
            animatedBoshy->SetState(Character::MoveState::RUN_LEFT);
        } else {
            animatedBoshy->SetState(Character::MoveState::IDLE_LEFT);
        }
    } else {
        // No horizontal movement key pressed, set to idle if not jumping
        if (!m_isJumping) {
            if (animatedBoshy->GetDirection() == Character::direction::LEFT) {
                animatedBoshy->SetState(Character::MoveState::IDLE_LEFT);
            } else {
                animatedBoshy->SetState(Character::MoveState::IDLE);
            }
        }
    }
    
    // Vertical movement (UP/DOWN keys were for direct Y change, typically not in platformers)
    // The original AppUpdate.cpp had UP/DOWN key logic; omitting for now unless specified,
    // as platformers usually only have jump for upward movement.

    // Shooting
    // shootCooldown was already updated at the start of this function.
    if (m_shootCooldown <= 0 && Util::Input::IsKeyPressed(Util::Keycode::X) && m_bullets_vector_ptr->size() < 5) {
        // Ensure m_root_renderer_ptr is valid if Bullet::CreateBullet adds to it.
        // Bullet::CreateBullet needs position, direction, lifetime, root.
        // App::Update used m_Boshy->GetPosition() and m_Boshy->GetDirection().
        // Here, playerPosition is m_Boshy's position, and m_boshy_ptr can give direction.
        auto bullet = Bullet::CreateBullet(playerPosition, m_boshy_ptr->GetDirection(), 2.0f, *m_root_renderer_ptr);
        m_bullets_vector_ptr->push_back(bullet);
        m_shootCooldown = 0.5f; // Cooldown period
    }

    // Jumping
    if (Util::Input::IsKeyDown(Util::Keycode::Z)) {
        if (m_jumpCount < 2 || godMode) {
            m_velocityY = jumpPower;
            m_isJumping = true; // Set jumping state
            if (!godMode) {
                m_jumpCount++;
            }
            // Set animation state for jump
            if (animatedBoshy->GetDirection() == Character::direction::LEFT) {
                animatedBoshy->SetState(Character::MoveState::JUMP_LEFT);
            } else {
                animatedBoshy->SetState(Character::MoveState::JUMP);
            }
        }
    } else if (Util::Input::IsKeyUp(Util::Keycode::Z) && m_isJumping && m_velocityY > 0) {
        // Short hop: reduce velocityY if Z is released early during upward motion
        m_velocityY *= 0.4f;
        // m_isJumping remains true until peak or landing
    }

    // Update animation if jump has peaked or character has landed
    if (m_velocityY <= 0 && m_isJumping) { // if falling or landed
        // If we landed (velocityY becomes 0 due to ground collision), m_isJumping would be set to false there.
        // This mainly handles changing animation state when starting to fall after a jump.
        // If not on ground, it's JUMP_FALL (if such state exists) or just keep JUMP state.
        // For simplicity, if no specific fall animation, idle/run will be set if not moving.
    }

    // If animation ended or not jumping and on ground (velocityY is 0 from landing)
    // The original logic: else if (animatedBoshy->IfAnimationEnds() || (animatedBoshy->GetState() != Character::MoveState::JUMP && velocityY == 0))
    // This needs to be carefully adapted.
    // If on ground (m_velocityY == 0 from landing collision and m_isJumping == false) and no input, go to IDLE.
    // The horizontal input handling above already sets IDLE/RUN.
    // Jump animations should typically loop or transition to fall animation.
    // IfAnimationEnds() might be for things like attack animations.
    // For now, the horizontal input block handles idle/run when not jumping.
    // And jump animations are set during jump.
}