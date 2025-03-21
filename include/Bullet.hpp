//
// Created by andyl on 2025/3/10.
//

#ifndef BULLET_HPP
#define BULLET_HPP
#include "Util/GameObject.hpp"
#include "Util/Animation.hpp"
#include "character.hpp"


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
    Character::direction GetDirection() const { return m_Direction; }

private:
    std::shared_ptr<Util::Animation> m_Bullet;
    std::string ImagePath;
    float lifeTime = 0.0f;
    bool m_IsVisible = true;
    Character::direction m_Direction;
};
#endif //BULLET_HPP
