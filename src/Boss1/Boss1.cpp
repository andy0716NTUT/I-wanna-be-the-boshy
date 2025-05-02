#include "../include/Boss1/Boss1.hpp"


Boss1::Boss1() {
    std::vector<std::string> frames ;
    for (int i = 1 ; i <= 2 ; i++) {
        frames.push_back(RESOURCE_DIR"/Image/enemy/Boss1/boss" + std::to_string(i) + ".png");
    }
    m_Animation = std::make_shared<Util::Animation>(frames, true, 300, true);
    m_Animation->Play();
    m_Drawable = m_Animation;

    m_Transform.translation.x = 500;  // 固定 X
    m_Transform.translation.y = -300; // 出場起始位置
}
glm::vec2 Boss1::GetPosition() {
    return m_Transform.translation;
}
void Boss1::SetHealth(float damage) {
    this->Health -= damage;
}

float Boss1::GetHealth() {
    return  this->Health;
}

void Boss1::Spawn(float deltaTime) {

    m_AttackType = AttackType::SPAWN;  // 呼叫一次 → 切換到 SPAWN 狀態
    m_Transform.translation.y = -300.0f;  // 重設起始

    m_SpawnY = -300.0f;  // 重設起始
    m_WaitTimer = 0.0f;


}
void Boss1::Update(float deltaTime) {
    if (m_AttackType == AttackType::SPAWN) {
        float targetY = 0.0f;
        float speed = 100.0f;

        if (m_SpawnY < targetY) {
            m_SpawnY += speed * deltaTime;
            if (m_SpawnY >= targetY) {
                m_SpawnY = targetY;
            }
        } else {
            m_WaitTimer += deltaTime;
            if (m_WaitTimer >= 3.0f) {
                m_AttackType = AttackType::TYPEA;  // 出場完成 → 切 TYPEA
            }
        }
        m_Transform.translation.y = m_SpawnY;
    }
    else if (m_AttackType == AttackType::TYPEA) {
        const float amplitude = 350.0f;
        const float frequency = 2.0f;
        m_Angle += frequency * deltaTime;
        m_Transform.translation.y = sin(m_Angle) * amplitude;
    }
}

