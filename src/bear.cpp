#include "bear.hpp"


bear::bear() {
    std::vector<std::string> frame1;
    std::vector<std::string> frame2;
    frame1.push_back(RESOURCE_DIR"/Image/enemy/bear1");
    frame1.push_back(RESOURCE_DIR"/Image/enemy/bear2");
    frame2.push_back(RESOURCE_DIR"/Image/enemy/bear3");
    frame2.push_back(RESOURCE_DIR"/Image/enemy/bear4");
    m_Animation_left = std::make_shared<Util::Animation>(frame1,false,100,true);
    m_Animation_right = std::make_shared<Util::Animation>(frame2,false,100,true);
}
bool bear::detect(std::string &phase) {
    if ((phase == "4" || phase.find("4_")) && !this->spawn) {
        this->spawn = true;
        this->attack = AttackType::CHASE;
        return true;
    }else if (phase == "5") {
        this->attack = AttackType::ROUND;
        return true;
    }
    return false;
}
bool bear::exist() {
    return this->spawn;
}
glm::vec2 bear::GetPosition() {
    return this->m_Transform.translation;
}
void bear::SetPosition(glm::vec2 position) {
    m_Transform.translation = position;
}

void bear::Update(glm::vec2 playerPosition) {
    switch (this->attack) {
        case AttackType::CHASE:
            if (m_Transform.translation.x - playerPosition.x > 0) {
                this->dir = direction::left;
            } else {
                this->dir = direction::right;
            }
            break;
        case AttackType::ROUND:
            // Implement round attack behavior
            break;
    }
}

