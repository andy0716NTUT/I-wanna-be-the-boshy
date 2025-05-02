//
// Created by andyl on 2025/5/2.
//

#ifndef BOSS1_HPP
#define BOSS1_HPP

#include "Util/GameObject.hpp"
#include "Util/Animation.hpp"


class Boss1 : public Util::GameObject{
private:
    std::shared_ptr<Util::Animation> m_Boss;
    std::shared_ptr<Util::Animation> m_BossBullet;
    std::shared_ptr<Util::Animation> m_mortar;

public:

    enum class AttackSate {
        SHOOT,
        LASER,
        MORTAR
    };

    Boss1();

    Update(float deltaTime);

};
#endif //BOSS1_HPP
