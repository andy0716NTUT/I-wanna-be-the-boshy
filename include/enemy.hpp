#ifndef ENEMY_HPP
#define ENEMY_HPP

#include <map> // 添加此行解决 'map' 未定义问题
#include "Util/GameObject.hpp"
#include "Util/Animation.hpp"
#include "Character.hpp"
#include "MapInfoLoader.hpp"
#include "Bullet.hpp"

class Enemy : public Util::GameObject {
public:
    enum class EnemyType {
        BASIC,      // 基本敵人
        FLYING,     // 飛行敵人  
        SHOOTER     // 射擊敵人
    };
    
    enum class MoveState {
        IDLE,
        WALK,
        WALK_LEFT,  // 新增左走狀態
        ATTACK,
        DAMAGED,
        DEAD
    };
    EnemyType GetType() const { return m_Type; }
    [[nodiscard]] const glm::vec2& GetPosition() const { return m_Transform.translation; }
    void SetPosition(const glm::vec2& position);
    
    void SetHealth(int health) { m_Health = health; }
    int GetHealth() const { return m_Health; }
    
    void SetSpeed(float speed) { m_Speed = speed; }
    void SetDirection(Character::direction dir) { m_Direction = dir; }
    Character::direction GetDirection() const { return m_Direction; }
    
    void SetState(MoveState state);
    MoveState GetState() const { return m_State; }
    
    void SetVisible(bool visible) { m_IsVisible = visible; }
    bool IsVisible() const { return m_IsVisible; }
    
    bool TakeDamage(int damage);
    void Update(float deltaTime, const std::shared_ptr<MapInfoLoader>& mapLoader);
    bool CheckBulletCollision(const std::vector<std::shared_ptr<Bullet>>& bullets);
    bool CheckPlayerCollision(const glm::vec2& playerPos);
    
    static std::vector<std::shared_ptr<Enemy>> CreateFromMap(
        const std::shared_ptr<MapInfoLoader>& mapLoader,
        Util::Renderer& renderer);
    
    static std::shared_ptr<Enemy> CreateEnemy(
        const glm::vec2& position,
        EnemyType type,
        Util::Renderer& renderer);

private:
    void LoadAnimations(EnemyType type);
    void UpdateMovement(float deltaTime, const std::shared_ptr<MapInfoLoader>& mapLoader);
    // 添加此行以声明 IsValidPosition 方法
    bool IsValidPosition(const glm::vec2& position, const std::shared_ptr<MapInfoLoader>& mapLoader);

    std::map<MoveState, std::shared_ptr<Util::Animation>> m_Animations;
    
    EnemyType m_Type = EnemyType::BASIC;
    MoveState m_State = MoveState::IDLE;
    Character::direction m_Direction = Character::direction::RIGHT;
    
    int m_Health = 1;
    float m_Speed = 2.0f;
    bool m_IsVisible = true;
    float m_VelocityY = 0.0f;
    const float m_Gravity = -0.5f;
    const float m_MaxFallSpeed = -10.0f;
    bool m_IsOnGround = false;
    // AI行為參數
    float m_PatrolTimer = 0.0f;
    float m_PatrolDistance = 100.0f;
    glm::vec2 m_StartPosition = {0.0f, 0.0f};
    float m_AttackCooldown = 0.0f;
};

#endif //ENEMY_HPP