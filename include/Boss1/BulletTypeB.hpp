//
// Created by andyl on 2025/5/4.
//

#ifndef BULLETTYPEB_HPP
#define BULLETTYPEB_HPP
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
class BulletTypeB : public Util::GameObject {
public:
    BulletTypeB(glm::vec2 position);
    void Update(float deltaTime);
    glm::vec2 GetPosition();
private:
    std::shared_ptr<Util::Image> m_Image;
    float m_MoveTimer = 0.0f;
};
#endif //BULLETTYPEB_HPP
