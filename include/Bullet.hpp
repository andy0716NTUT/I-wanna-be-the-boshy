//
// Created by andyl on 2025/3/10.
//

#ifndef BULLET_HPP
#define BULLET_HPP
#include "Util/GameObject.hpp"
#include "Util/Animation.hpp"


class Bullet : public Util::GameObject {
public:
    [[nodiscard]] const glm::vec2& GetPosition() const { return m_Transform.translation; }

    void SetImage(const std::string& ImagePath);

    void SetPosition(const glm::vec2& Position) { m_Transform.translation = Position; }


private:
    std::shared_ptr<Util::Animation> m_Bullet;
    std::string ImagePath;
    float flyingSpeed = 5.0f;
};
#endif //BULLET_HPP
