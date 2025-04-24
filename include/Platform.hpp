//
// Created by andyl on 2025/4/18.
//

#ifndef PLATFORM_HPP
#define PLATFORM_HPP
#include "Util/GameObject.hpp"
class Platform : public  Util::GameObject{
public:
    void SetPosition(glm::vec2 position){m_Transform.translation = position ;}
    void SetImage(std::string ImagePath);
};
#endif //PLATFORM_HPP
