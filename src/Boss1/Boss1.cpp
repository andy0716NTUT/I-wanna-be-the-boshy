#include "../include/Boss1/Boss1.hpp"

Boss1::Boss1() {
    std::vector<std::string> frames;
    for (int i = 1; i <= 2; i++) {
        frames.push_back(RESOURCE_DIR "/Image/Boss1/boss" + std::to_string(i) + ".png");
    }
    m_Animation = std::make_shared<Util::Animation>(frames, true, 300, true);
    m_Animation->Play();
    m_Drawable = m_Animation;
    m_Transform.translation.x = 500;
    m_Transform.translation.y = -300;
}

glm::vec2 Boss1::GetPosition() {
    return m_Transform.translation;
}

void Boss1::SetHealth(float damage) {
    this->Health -= damage;
}

float Boss1::GetHealth() {
    return this->Health;
}

void Boss1::Spawn(float deltaTime) {
    m_AttackType = AttackType::SPAWN;
    m_Transform.translation.y = -300.0f;
    m_SpawnY = -300.0f;
    m_WaitTimer = 0.0f;
}

void Boss1::Update(float deltaTime, glm::vec2 playerPosition, Util::Renderer& rootRenderer) {
    if (m_AttackType == AttackType::SPAWN) {
        float targetY = 0.0f;
        float speed = 100.0f;
        if (m_SpawnY < targetY) {
            m_SpawnY += speed * deltaTime;
            if (m_SpawnY >= targetY) m_SpawnY = targetY;
        } else {
            m_WaitTimer += deltaTime;
            if (m_WaitTimer >= 3.0f) m_AttackType = AttackType::TYPEA;
        }
        m_Transform.translation.y = m_SpawnY;
    } else if (m_AttackType != AttackType::SPAWN) {
        const float amplitude = 350.0f;
        const float frequency = 2.0f;
        m_Angle += frequency * deltaTime;
        m_Transform.translation.y = sin(m_Angle) * amplitude;

        switch (m_AttackType) {
            case AttackType::TYPEA:
                m_ShootTimer += deltaTime;
            if (TypeAShootCount < 8 && m_ShootTimer >= 1.0f) {
                auto bullet = std::make_shared<BulletTypeA>(GetPosition(), playerPosition);
                rootRenderer.AddChild(bullet);
                m_BulletsA.push_back(bullet);
                m_ShootTimer = 0.0f;
                TypeAShootCount++;
            }

            // 更新所有子彈
            for (auto& bullet : m_BulletsA) {
                bullet->Update(deltaTime);
            }

            // 檢查是否所有子彈都飛出場外
            if (TypeAShootCount >= 8) {
                bool allBulletsOutOfScreen = true;
                for (auto& bullet : m_BulletsA) {
                    if (bullet->GetPosition().x >= -640.0f) {
                        allBulletsOutOfScreen = false;
                        break;
                    }
                }
                if (allBulletsOutOfScreen) {
                    m_AttackType = AttackType::LIGHTATTACK;
                    m_BulletsA.clear();  // 清空子彈列表（可選）
                }
            }

                break;
            default:
                break;

        }
    }
    m_Transform.translation.x = std::round(m_Transform.translation.x);
    m_Transform.translation.y = std::round(m_Transform.translation.y);
}


