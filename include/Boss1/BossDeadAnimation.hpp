//
// Created by andyl on 2025/5/7.
//

#ifndef BOSSDEADANIMATION_HPP
#define BOSSDEADANIMATION_HPP

#include "Util/Animation.hpp"
#include "Util/GameObject.hpp"

class BossDeadAnimation : public Util::GameObject{
private:
    std::shared_ptr<Util::Animation> m_Animation;
public:
    BossDeadAnimation();
    void SetPOsition(glm::vec2 position);
};



#endif //BOSSDEADANIMATION_HPP
