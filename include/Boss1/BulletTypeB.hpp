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
    float m_StartY;
    float m_TargetY;
    float m_BounceY;     // 回彈高度 (Y 方向)
    float m_AnimTimer = 0.0f;
    float m_AnimDuration = 0.5f;
    int m_MoveDirectionX = 0;
    float m_StartX;
    float m_TargetX;
};
#endif //BULLETTYPEB_HPP
