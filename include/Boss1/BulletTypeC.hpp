//
// Created by andyl on 2025/5/5.
//

#ifndef BULLETTYPEC_HPP
#define BULLETTYPEC_HPP
#include "Util/Animation.hpp"
#include "Util/GameObject.hpp"

class BulletTypeC : public Util::GameObject {
public:
    BulletTypeC(glm::vec2 bossPosition,glm::vec2 direction);
    void Update(float deltaTime);
    glm::vec2 GetPosition();
private:
    std::shared_ptr<Util::Animation> m_Animation;
    glm::vec2 m_Direction;
    float m_Speed = 150.0f;
    float m_BaseY;
    float m_ElapsedTime = 0.0f;
    float m_SpeedX = 200.0f;
    float m_Amplitude = 40.0f;
    float m_Frequency = 4.0f;
};
#endif //BULLETTYPEC_HPP
