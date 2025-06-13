#ifndef BULLET_HPP
#define BULLET_HPP
#include "Util/GameObject.hpp"
#include "Util/Animation.hpp"
#include "character.hpp"
#include "MapInfoLoader.hpp"
#include "MapObject/CheckPoint.hpp"

class Bullet : public Util::GameObject {
public:
    [[nodiscard]] const glm::vec2& GetPosition() const { return m_Transform.translation; }

    void SetImage(const std::string& ImagePath);
    void SetPosition(const glm::vec2& Position);
    void Update(float deltaTime);
    bool IsVisible() const { return m_IsVisible; }
    void SetVisible(bool visible) { m_IsVisible = visible; }
    void SetLifeTime(float time) { lifeTime = time; }    void SetDirection(Character::direction dir) { m_Direction = dir; }
    void SetTargetPosition(const glm::vec2& targetPos) { m_TargetPosition = targetPos; }
    static void CleanBullet(std::vector<std::shared_ptr<Bullet>>& bullets);
    static void CleanBullet(std::vector<std::shared_ptr<Bullet>>& bullets, Util::Renderer& renderer);
    [[nodiscard]] bool ShouldBeRemoved() const { return !m_IsVisible; }
    Character::direction GetDirection() const { return m_Direction; }
    
    // 更新檢查點子彈的位置（朝向玩家移動）
    bool UpdateCheckpointBullet(float deltaTime);

    // 新增的方法
    // 在地图中更新子弹位置并处理碰撞
    bool UpdateWithCollision(float deltaTime, const std::shared_ptr<MapInfoLoader>& mapLoader);

    // 检查与检查点的碰撞
    bool CheckCheckpointCollision(const std::vector<std::shared_ptr<CheckPoint>>& checkpoints,
                                  glm::vec2& outCheckpointPos, std::string& outPhase, int& outX, int& outY);    // 创建新子弹的工厂方法
    static std::shared_ptr<Bullet> CreateBullet(
        const glm::vec2& position,
        Character::direction direction,
        float lifeTime,
        Util::Renderer& renderer);
          // 创建检查点子弹的工厂方法（子彈會使角色死亡）
    static std::shared_ptr<Bullet> CreateCheckpointBullet(
        const glm::vec2& position,
        const glm::vec2& targetPosition,
        float lifeTime,
        Util::Renderer& renderer);// 子弹速度常量
    static constexpr float BULLET_SPEED = 15.0f;
    static constexpr float GRAVITY = -0.8f; // 重力加速度（向下為負值）

    void SetDirectionVector(const glm::vec2& dirVec) { m_DirectionVector = dirVec; }

private:
    std::shared_ptr<Util::Animation> m_Bullet;
    std::string ImagePath;
    float lifeTime = 0.0f;
    bool m_IsVisible = true;
    Character::direction m_Direction;    glm::vec2 m_TargetPosition = {0, 0}; // 目標位置（用於追蹤子彈）
    float m_VelocityY = 0.0f; // 垂直速度（用於重力效果）
    bool m_IsCheckpointBullet = false; // 标记是否为checkpoint子弹
    glm::vec2 m_DirectionVector = {1.0f, 0.0f}; // 移动方向向量（用于checkpoint子弹朝向目标移动）
};
#endif //BULLET_HPP