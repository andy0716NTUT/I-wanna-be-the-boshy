#ifndef PLAYER_MANAGER_HPP
#define PLAYER_MANAGER_HPP

#include <cstddef>
#include "glm/glm.hpp"

class AnimatedCharacter;

struct PlayerRuntimeState {
    float velocityY = 0.0f;
    int jumpCount = 0;
    float shootCooldown = 0.0f;
    bool isJumping = false;
};

struct PlayerMovementInput { bool right, left, up, down; };
struct PlayerTileContacts { int above, below, left, right; };
enum class PlayerFeel { Original, Weighted };

// Values remain in original per-update units. Respawn intentionally does not
// reset this state: the original function statics survived death and R too.
class PlayerManager {
public:
    static constexpr float FrameSeconds = 1.0f / 60.0f;
    static constexpr std::size_t MaxBullets = 5;
    PlayerRuntimeState& GetState() { return m_State; }
    const PlayerRuntimeState& GetState() const { return m_State; }
    int GetJumpCount() const { return m_State.jumpCount; }
    PlayerFeel GetFeel() const { return m_Feel; }
    void SetFeel(PlayerFeel feel) { m_Feel = feel; }
    // Sample once before both original gravity additions: an ascent that
    // crosses zero this update must still have its original trajectory.
    float GravityForUpdate(float gravity) const {
        return m_Feel == PlayerFeel::Weighted && gravity < 0 && m_State.velocityY <= 0
            ? gravity * 1.6f : gravity;
    }

    void AddEntryGravity(float gravity) { m_State.velocityY += gravity; }
    void AdvanceCommonCooldown(float dt) { m_State.shootCooldown -= dt * 4; }
    void AdvanceAliveCooldown(float dt) { m_State.shootCooldown -= dt; }
    void IntegrateVertical(glm::vec2& position, float gravity);
    void ResolveTiles(glm::vec2& position, int tileY, int aboveTile, int belowTile, int centerTile);
    void Move(AnimatedCharacter& character, glm::vec2& position,
              const PlayerMovementInput& input, const PlayerTileContacts& tiles);
    void ShotFired() { m_State.shootCooldown = 0.5f; }
    void StartJump(bool godMode);
    void ShortenJump();
private:
    PlayerRuntimeState m_State;
    PlayerFeel m_Feel = PlayerFeel::Original;
};

#endif
