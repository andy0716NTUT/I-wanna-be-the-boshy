#include <iostream>

#include "MapObject/bird.hpp"

Bird::Bird()
    : m_Chasing(false), m_SpeedX(4.5f), m_SpeedY(10.0f) {

    std::vector<std::string> frames;
    for (int i = 1 ; i <= 2 ; i ++) {
        frames.push_back(RESOURCE_DIR"/Image/enemy/bird" + std::to_string(i) + ".png");
    }
    m_Animation = std::make_shared<Util::Animation>(frames, true, 300, true);
    m_Animation->Play();
    m_Drawable = m_Animation;
}

void Bird::Create( Util::Renderer& rootRenderer,glm::vec2 startPos) {
    auto birds = std::make_shared<Bird>();
    birds->Setposition(startPos);
    rootRenderer.AddChild(birds);
}
void Bird::StartChase() {
    m_Chasing = true;
}
void Bird::StopChase() {
    m_Chasing = false;
}
void Bird::StartPending(float delay, float lastY) {
    m_IsPending = true;
    m_PendingTimer = delay;
    m_PreviousY = lastY;
    this->SetVisible(false); // 暫時隱藏
}
void Bird::Update(float deltaTime, glm::vec2 playerPos) {
    if (m_IsPending) {
        m_PendingTimer -= deltaTime*60;
        std::cout << m_PendingTimer << std::endl;
        if (m_PendingTimer <= 0.0f) {
            m_IsPending = false;
            this->Setposition(glm::vec2(-640.0f, m_PreviousY)); // 從左邊出現
            this->SetVisible(true);
        }
        return; // Pending時直接return，不更新飛行
    }

    if (!m_Chasing) return;

    glm::vec2 pos = this->GetPosition();
    pos.x += m_SpeedX;

    // 追蹤Y座標
    if (playerPos.y > pos.y) {
        pos.y += m_SpeedY;
        if (pos.y > playerPos.y) pos.y = playerPos.y;
    } else if (playerPos.y < pos.y) {
        pos.y -= m_SpeedY;
        if (pos.y < playerPos.y) pos.y = playerPos.y;
    }

    this->Setposition(pos);
}
