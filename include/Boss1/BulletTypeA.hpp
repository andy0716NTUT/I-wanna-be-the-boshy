//
// Created by andyl on 2025/5/3.
//

#ifndef BULLETTYPEA_HPP
#define BULLETTYPEA_HPP
// BulletTypeA.hpp
#pragma once
#include "Util/GameObject.hpp"
#include "Util/Animation.hpp"

class BulletTypeA : public Util::GameObject {
public:
    BulletTypeA(glm::vec2 bossPosition, glm::vec2 playerPosition);
    void Update(float deltaTime);
    glm::vec2 GetPosition();

private:
    glm::vec2 m_Velocity;
    std::shared_ptr<Util::Animation> m_Animation;
};

#endif //BULLETTYPEA_HPP
