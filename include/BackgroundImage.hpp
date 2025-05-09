//
// Created by andyl on 2025/3/8.
//

#ifndef BACKGROUNDIMAGE_HPP
#define BACKGROUNDIMAGE_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"


class BackgroundImage : public Util::GameObject {
private:
    inline std::string ImagePath(const std::string phase,std::string Currentworld) {
        return RESOURCE_DIR"/Image/Background/" + Currentworld +"/phase" + phase + ".png";
    }
public:
    BackgroundImage() : GameObject(std::make_unique<Util::Image>(RESOURCE_DIR"/Image/Background/phase1.png"),-10){}

    void SetPhase(std::string phase,std::string CurrentWorld) {
        auto temp = std::dynamic_pointer_cast<Util::Image>(m_Drawable);
        temp->SetImage(ImagePath(phase,CurrentWorld));
    }
    void rotate(float deltaTime) {
        m_Transform.rotation += (M_PI * deltaTime)/3; // Rotate π radians per second (half circle per second)
    }

    void resetRotation() {
        m_Transform.rotation = 0.0f; // Reset rotation to initial state
    }

    float getRotation() const {
        return m_Transform.rotation;
    }
};



#endif //BACKGROUNDIMAGE_HPP
