//
// Created by andyl on 2025/5/16.
//

#ifndef SPIDER_HPP
#define SPIDER_HPP
#include <memory>
#include <glm/vec2.hpp>

#include "Util/Animation.hpp"
#include "Util/GameObject.hpp"


class spider : public Util::GameObject{
public:
    spider();
    bool detect(glm::vec2 position);
    void Update();
private:
    std::shared_ptr<Util::Animation> m_animation;
    std::string dir = "down";
    bool exist = false;
};



#endif //SPIDER_HPP
