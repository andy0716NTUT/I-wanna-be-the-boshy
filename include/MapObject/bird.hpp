#ifndef BIRD_HPP
#define BIRD_HPP

#include "Util/GameObject.hpp"
#include "Util/Animation.hpp"
#include "Util/Renderer.hpp"
class Bird : public Util::GameObject {
public:
    Bird();

    void Create(Util::Renderer& rootRenderer,glm::vec2 startPos);
    void Update(float deltaTime, glm::vec2 playerPos);
    void StartChase(); // 開始追逐
    void StopChase();  // 停止追逐
    void Setposition(glm::vec2 position) {
        m_Transform.translation = position;
    }
    glm::vec2 GetPosition() {
        return m_Transform.translation;
    }
    float GetSpeedX(){return this->m_SpeedX;}
    void StartPending(float delay, float lastY);
private:
    std::shared_ptr<Util::Animation> m_Animation;
    bool m_IsPending = false;
    float m_PendingTimer = 0.0f;
    float m_PreviousY = 0.0f; // 上一張地圖的Y座標
    bool m_Chasing;
    float m_SpeedX;
    float m_SpeedY;
};

#endif // BIRD_HPP
