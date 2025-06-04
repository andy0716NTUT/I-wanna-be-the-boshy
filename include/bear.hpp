#include "Util/GameObject.hpp"
#include "Util/Animation.hpp"
#include "MapInfoLoader.hpp"

class bear : public Util::GameObject {
public:
    enum class direction {
        left,
        right
    };
    enum class AttackType {
        CHASE,
        ROUND
    };
    bear();
    bool detect(std::string& phase);
    glm::vec2 GetPosition();
    void SetPosition(glm::vec2 position);
    bool exist();
    void Update(glm::vec2 playerPosition);
    void SetMapInfoLoader(std::shared_ptr<MapInfoLoader> mapLoader) { m_MapInfoLoader = mapLoader; }
    void SetCurrentPhase(const std::string& phase) { m_CurrentPhase = phase; }
private:
    std::shared_ptr<Util::Animation> m_Animation_left;
    std::shared_ptr<Util::Animation> m_Animation_right;
    std::shared_ptr<MapInfoLoader> m_MapInfoLoader;
    std::string m_CurrentPhase;
    direction dir = direction::left;
    AttackType attack = AttackType::CHASE;
    bool spawn = false;    float m_velocityY = 0.0f;
    bool m_onGround = false;
    const float m_Gravity = -0.5f;
    const float m_MaxFallSpeed = -10.0f;
    float m_currentSpeed = 0.0f;
    float m_targetSpeed = 0.0f;
    float m_turnCooldown = 0.0f;

    // 跳躍參數
    bool m_isJumping = false;
    float m_jumpTime = 0.0f;
    float m_jumpCooldown = 0.0f;
    float m_jumpVelocity = 0.0f;
    float m_jumpY = -148.0f;

};