//
// Created by andyl on 2025/5/3.
//

#ifndef LIGHTATTACK_HPP
#define LIGHTATTACK_HPP
#include "Util/GameObject.hpp"
#include "Util/Animation.hpp"
class LightAttack : public Util::GameObject {
public:
    LightAttack(glm::vec2 playerPosition);
    enum class state{WAITING,SHOOT,Disappear};
    void Update(float deltaTime);
    bool HitsPlayer(glm::vec2 playerPosition) const;
    glm::vec2 GetPosition() const;
    bool IsActive() const;
private:
    std::shared_ptr<Util::Animation> m_Animation_Waiting;
    std::shared_ptr<Util::Animation> m_Animation_Shoot;
    std::shared_ptr<Util::Animation> m_Animation_Disappear;
    bool m_HasSwitched = false;
    float m_WaitTimer = 0.0f;
    bool m_HasPlayedDisappear = false;

};
#endif //LIGHTATTACK_HPP
