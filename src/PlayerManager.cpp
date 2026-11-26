#include "PlayerManager.hpp"
#include "AnimatedCharacter.hpp"
#include <optional>

void PlayerManager::IntegrateVertical(glm::vec2& position, float gravity) {
    // Keep the second gravity addition, then clamp, then integrate.
    m_State.velocityY += gravity;
    // Weighted is an opt-in feel preview; both limits stay below one16px tile.
    const float maxFallSpeed = m_Feel == PlayerFeel::Weighted ? -14.0f : -10.0f;
    if (m_State.velocityY < maxFallSpeed) m_State.velocityY = maxFallSpeed;
    position.y += m_State.velocityY;
}

void PlayerManager::ResolveTiles(glm::vec2& position, int tileY,
                                 int aboveTile, int belowTile, int centerTile) {
    auto& velocityY = m_State.velocityY;
    auto& jumpCount = m_State.jumpCount;
    if ((aboveTile == 2) && velocityY > 0){
        position.y = 480 - ((tileY + 1) * 16);
        velocityY = 0;
    }
    if ((belowTile == 1 || belowTile == 8)  && velocityY < 0){
        position.y = 480 - ((tileY) * 16) - 4;
        velocityY = 0;
        jumpCount = 0;
    }
    if (centerTile == 1 && velocityY >= 0){
        position.y = 480 - (tileY - 1) * 16;
        velocityY = 0;
    }
    if (!(belowTile == 1 || belowTile == 8)) {
        if (jumpCount == 0) jumpCount = 1;
    }
}

void PlayerManager::Move(AnimatedCharacter& character, glm::vec2& position,
                         const PlayerMovementInput& input, const PlayerTileContacts& tiles) {
    using State = Character::MoveState;
    using Direction = Character::direction;
    const auto axis = [&](bool pressed, bool blocked, float& coordinate, float delta,
                          std::optional<Direction> facing, State moving, State idle) {
        if (!pressed) return;
        const float previous = coordinate;
        if (!blocked) {
            coordinate += delta;
            if (facing) character.SetDirection(*facing);
        }
        character.SetState(coordinate != previous ? moving : idle);
    };
    // Independent branches and this exact order preserve simultaneous-key behavior.
    axis(input.right, tiles.right == 2, position.x, 5, Direction::RIGHT, State::RUN, State::IDLE);
    axis(input.left, tiles.left == 2, position.x, -5, Direction::LEFT, State::RUN_LEFT, State::IDLE_LEFT);
    axis(input.up, tiles.above == 2, position.y, 5, std::nullopt, State::RUN, State::IDLE);
    axis(input.down, tiles.below == 2, position.y, -5, std::nullopt, State::RUN, State::IDLE);
}

void PlayerManager::StartJump(bool godMode) {
    m_State.velocityY = 16;
    m_State.isJumping = true;
    if (!godMode) ++m_State.jumpCount;
}

void PlayerManager::ShortenJump() {
    m_State.velocityY *= 0.4f;
    m_State.isJumping = false;
}
