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
        SHOOT_LEFT,
        DEATH,      // 添加死亡状态
        DEATH_LEFT  // 添加朝左的死亡状态
    };

    explicit Character(const std::string& ImagePath);

    [[nodiscard]] MoveState GetState() const { return m_CurrentState; }

    void SetState(MoveState state) { m_CurrentState = state; }

    [[nodiscard]] const glm::vec2& GetPosition() const { return m_ActualPosition; }

    void SetPosition(const glm::vec2& Position) { 
        m_ActualPosition = Position;
        m_Transform.translation = Position;
    }

    void SetImage(const std::string& ImagePath);

    void SetDirection(direction direct){m_direction = direct;}

    direction GetDirection(){return m_direction;}

    void UpdatePositionWithRotation(float rotation) {
        // 保存原始位置
        glm::vec2 originalPos = m_ActualPosition;
        
        // 計算相對於 (0,0) 的向量
        float dx = originalPos.x;
        float dy = originalPos.y;
        
        // 計算旋轉後的新位置（只用於視覺效果）
        float newX = dx * std::cos(rotation) - dy * std::sin(rotation);
        float newY = dx * std::sin(rotation) + dy * std::cos(rotation);
        
        // 更新視覺位置
        m_Transform.translation.x = newX;
        m_Transform.translation.y = newY;
        
        // 設置旋轉角度
        m_Transform.rotation = rotation;
    }

private:
    float m_VelocityY = 0.0f;
    float m_Gravity = -0.98f * 1.5f;
    float m_JumpPower = 15.0f;
    int m_JumpCount = 0;
    bool m_OnGround = false;
    float m_MaxFallSpeed = -10.0f;
    void ResetPosition() { 
        m_ActualPosition = {0, 0};
        m_Transform.translation = {0, 0}; 
    }
    direction m_direction = direction::RIGHT;
    std::string m_ImagePath;
    MoveState m_CurrentState = MoveState::IDLE;
    glm::vec2 m_ActualPosition = {0, 0}; // 存儲實際位置
};

#endif //CHARACTER_HPP