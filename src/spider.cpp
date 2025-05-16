//
// Created by andyl on 2025/5/16.
//

#include "../include/MapObject/spider.hpp"
spider::spider() {
    std::vector<std::string> frame;
    frame.push_back(RESOURCE_DIR"/Image/enemy/spider1");
    frame.push_back(RESOURCE_DIR"/Image/enemy/spider1");
    m_animation = std::make_shared<Util::Animation>(frame,false,100,true);
    m_Transform.translation = {0.0f,600.0f};


}
bool spider::detect(glm::vec2 position) {
    if (position.x >= -100.0f) {
        this->SetDrawable(m_animation);
        m_animation->Play();
        this->exist = true;
        return true;
    }
    return false;
}
void spider::Update() {
    if (exist) {
        if (this->dir == "down")m_Transform.translation.y -= 100.0f;
        if (m_Transform.translation.y < -500.0f)this->dir = "up";
        if (this->dir == "up")m_Transform.translation.y += 100.0f;
    }

}
