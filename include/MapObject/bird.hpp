//
// Created by andyl on 2025/4/28.
//

#ifndef BIRD_HPP
#define BIRD_HPP
#include "Util/GameObject.hpp"
#include "Util/Animation.hpp"
class bird : public Util::GameObject{
public:
    bird() {
      std::vector<std::string> frames;
      for (int i = 0 ; i < 2 ; i++) {
            frames.push_back(RESOURCE_DIR"Image/enemy/bird" +  std::to_string(i) + ".png");
      }
      m_Animation = std::make_shared<Util::Animation>(frames, true, 100, false);
      m_Animation->Pause();
    }
    void play() {
      m_Drawable = m_Animation;
      m_Animation->Play();
    }
    void CheckCurrentPhase(std::string CurrentPhase, float deltaTIme,Util::Renderer root) {
        AppearTime += deltaTIme;
        if (CurrentPhase == "8" && AppearTime > 1.5f && !isActivate) {

        }

    }
    void SetPosition(glm::vec2 position) {
      m_Transform.translation = position;
    }
    void Update(glm::vec2 playerPosition) {
        if (isActivate) {
          this->m_Transform.translation.y  = playerPosition.y;
          this->m_Transform.translation.x += 5 ;
        }
    }
  bool GetActivate(){return isActivate;}
 private:
   std::shared_ptr<Util::Animation> m_Animation;
   float AppearTime = 0.0f;
   bool isActivate = false;
};
#endif //BIRD_HPP
