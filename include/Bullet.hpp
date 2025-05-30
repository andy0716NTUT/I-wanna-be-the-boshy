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
    void SetLifeTime(float time) { lifeTime = time; }
    void SetDirection(Character::direction dir) { m_Direction = dir; }
    static void CleanBullet(std::vector<std::shared_ptr<Bullet>>& bullets);
    [[nodiscard]] bool ShouldBeRemoved() const { return !m_IsVisible; }
    Character::direction GetDirection() const { return m_Direction; }

    // 新增的方法
    // 在地图中更新子弹位置并处理碰撞
    bool UpdateWithCollision(float deltaTime, const std::shared_ptr<MapInfoLoader>& mapLoader);

    // 检查与检查点的碰撞
    bool CheckCheckpointCollision(const std::vector<std::shared_ptr<CheckPoint>>& checkpoints,
                                  glm::vec2& outCheckpointPos, std::string& outPhase, int& outX, int& outY);

    // 创建新子弹的工厂方法
    static std::shared_ptr<Bullet> CreateBullet(
        const glm::vec2& position,
        Character::direction direction,
        float lifeTime,
        Util::Renderer& renderer);

    // 子弹速度常量
    static constexpr float BULLET_SPEED = 15.0f;

private:
    std::shared_ptr<Util::Animation> m_Bullet;
    std::string ImagePath;
    float lifeTime = 0.0f;
    bool m_IsVisible = true;
    Character::direction m_Direction;
};
#endif //BULLET_HPP