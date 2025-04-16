//
// Created by andyl on 2025/4/12.
//

#ifndef CHECKPOINT_HPP
#define CHECKPOINT_HPP

#include "AnimatedCharacter.hpp"

class CheckPoint : public AnimatedCharacter{
private:
    glm::vec2 position ;
public:
    glm::vec2 GetPosition() {
        return  this->position;
    }
    void SetPosition(glm::vec2 position) {
        m_Transform.translation = position;
        this->position = position;
    }

    void SetImage(std::string& ImagePhth);
};
#endif //CHECKPOINT_HPP
