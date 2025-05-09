#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include <string>
#include "Util/GameObject.hpp"
#include "MapInfoLoader.hpp"
class Character : public Util::GameObject {
public:
    enum class direction {
        RIGHT,
        LEFT
    };
    enum class MoveState {
        IDLE,
        IDLE_LEFT,
        JUMP,
        JUMP_LEFT,
        RUN,
        RUN_LEFT,
        SHOOT,
        SHOOT_LEFT
    };

    explicit Character(const std::string& ImagePath);

    [[nodiscard]] MoveState GetState() const { return m_CurrentState; }

    void SetState(MoveState state) { m_CurrentState = state; }

    [[nodiscard]] const glm::vec2& GetPosition() const { return m_Transform.translation; }

    void SetPosition(const glm::vec2& Position) { m_Transform.translation = Position; }

    void SetImage(const std::string& ImagePath);

    void SetDirection(direction direct){m_direction = direct;}

    direction GetDirection(){return m_direction;}

    void UpdatePositionWithRotation(float rotation) {
        // 只更新繪製時的旋轉角度，不改變實際位置
        m_Transform.rotation = rotation;
    }

private:
    float m_VelocityY = 0.0f;
    float m_Gravity = -0.98f * 1.5f;
    float m_JumpPower = 15.0f;
    int m_JumpCount = 0;
    bool m_OnGround = false;
    float m_MaxFallSpeed = -10.0f;
    void ResetPosition() { m_Transform.translation = {0, 0}; }
    direction m_direction = direction::RIGHT;
    std::string m_ImagePath;
    MoveState m_CurrentState = MoveState::IDLE;
};

#endif //CHARACTER_HPP