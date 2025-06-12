#include "../include/Boss1/Boss1.hpp"

#include "GameObjectHelper.hpp"
#include "Util/Logger.hpp"

Boss1::Boss1() {
    std::vector<std::string> frames;
    for (int i = 1; i <= 2; i++) {
        frames.push_back(RESOURCE_DIR "/Image/Boss1/boss" + std::to_string(i) + ".png");
    }
    m_Animation = std::make_shared<Util::Animation>(frames, true, 300, true);
    m_Animation->Play();
    SetDrawable(m_Animation);



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

void Boss1::TakeDamage(int damage) {
    currentHp -= damage;
    if (currentHp < 0) currentHp = 0;

    float hpRatio = static_cast<float>(currentHp) / maxHp;
    m_HP->SetScaleX(hpRatio);  // BossHpInfo 裡要有這個函式控制 scale
}


void Boss1::Spawn(float deltaTime,Util::Renderer& rootRenderer) {
    m_AttackType = AttackType::SPAWN;
    m_Transform.translation.y = -300.0f;
    m_SpawnY = -300.0f;
    m_WaitTimer = 0.0f;
    m_HP = std::make_shared<BossHpInfo>();
    rootRenderer.AddChild(m_HP);
}
bool Boss1::IsDead() const {
    return this->currentHp <= 0 ;
}

bool Boss1::playerDead() {
    return this->isPlayerDead;
}

bool Boss1::Boss1Finished() {
    return DeadAnimationFinished;
}
void Boss1::ClearAnimation(Util::Renderer& rootRenderer) {
    if (m_DeadAnimation) {
        rootRenderer.RemoveChild(m_DeadAnimation);
        m_DeadAnimation.reset();
    }
    if(m_BulletsA.size() > 0) {
        ClearGameObjects(m_BulletsA);
        m_BulletsA.clear();
    }
    if(m_BulletsB.size() > 0) {
        ClearGameObjects(m_BulletsB);
        m_BulletsB.clear();
    }
    if(m_BulletsC.size() > 0) {
        ClearGameObjects(m_BulletsC);
        m_BulletsC.clear();
    }
    if(m_LightAttack) {
        ClearGameObjects(m_LightAttack);
        m_LightAttack.reset();
    }
    if(m_HP) {
        rootRenderer.RemoveChild(m_HP);
        m_HP.reset();
    }
    maxHp = 5;
    currentHp = 5;

}
void Boss1::CheckPlayerDeathByBullets(const glm::vec2& playerPosition) {
    const float DEATH_DISTANCE = 40.0f; // 距離閾值，可依需求調整

    for (const auto& bullet : m_BulletsA) {
        if (glm::distance(bullet->GetPosition(), playerPosition) < DEATH_DISTANCE) {
            isPlayerDead = true;
            return;
        }
    }
    for (const auto& bullet : m_BulletsB) {
        if (glm::distance(bullet->GetPosition(), playerPosition) < DEATH_DISTANCE) {
            isPlayerDead = true;
            return;
        }
    }
    for (const auto& bullet : m_BulletsC) {
        if (glm::distance(bullet->GetPosition(), playerPosition) < DEATH_DISTANCE) {
            isPlayerDead = true;
            return;
        }
    }
    if (m_LightAttack) { // 假設 LightAttack 有這樣的檢查
        if (glm::distance(m_LightAttack->GetPosition(), playerPosition) < DEATH_DISTANCE) {
            isPlayerDead = true;
            return;
        }
    }
}

void Boss1::Update(float deltaTime, glm::vec2 playerPosition, Util::Renderer& rootRenderer) {
    if (this->IsDead()) {
        if (!m_DeadAnimation) {
            ClearGameObjects(m_BulletsA);
            ClearGameObjects(m_BulletsB);
            ClearGameObjects(m_BulletsC);
            m_BulletsA.clear();
            m_BulletsB.clear();
            m_BulletsC.clear();
            m_DeadAnimation = std::make_shared<BossDeadAnimation>();
            rootRenderer.AddChild(m_DeadAnimation);
        }

        m_Transform.translation.y -= 100.0f * deltaTime;
        m_DeadAnimation->SetPOsition(this->m_Transform.translation);

        if (m_Transform.translation.y < -360) {
            DeadAnimationFinished = true;
        }
    }
    if (m_AttackType == AttackType::SPAWN) {
        if (Util::Input::IsKeyDown(Util::Keycode::S)) {
            m_AttackType = AttackType::TYPEA;
            return;
        }
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
    } else if (m_AttackType != AttackType::SPAWN && !IsDead()) {
        if (m_CanMoveVertically) {
            const float amplitude = 350.0f;
            const float frequency = 2.0f;
            m_Angle += frequency * deltaTime;
            m_Transform.translation.y = sin(m_Angle) * amplitude;
        }

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
                for (auto& bullet : m_BulletsA) {
                    bullet->Update(deltaTime);
                }
                if (TypeAShootCount >= 8) {
                    bool allBulletsOutOfScreen = true;
                    for (auto& bullet : m_BulletsA) {
                        if (bullet->GetPosition().x >= -700.0f) {
                            allBulletsOutOfScreen = false;
                            break;
                        }
                    }
                    if (allBulletsOutOfScreen) {
                        m_AttackType = AttackType::LIGHTATTACK;
                        TypeAShootCount = 0;
                        ClearGameObjects(m_BulletsA);
                        m_BulletsA.clear();
                    }
                }
                break;
            case AttackType::LIGHTATTACK: {
                if (!m_HasStartedLightAttack) {
                    m_CanMoveVertically = false;
                    m_HasStartedLightAttack = true;
                    m_LightAttackCount = 0;
                    m_LightAttackTimer = 0.0f;
                }
                float speedY = 100.0f;
                float dy = playerPosition.y - m_Transform.translation.y;
                float step = speedY * deltaTime;
                if (fabs(dy) <= step) {
                    m_Transform.translation.y = playerPosition.y;
                } else {
                    m_Transform.translation.y += (dy > 0 ? step : -step);
                }
                m_LightAttackTimer += deltaTime;
                if (m_LightAttackCount < 4 && m_LightAttackTimer >= 3.0f) {
                    m_CanMoveVertically = true;
                    m_Transform.translation.y = playerPosition.y;
                    if (m_LightAttack) {
                        rootRenderer.RemoveChild(m_LightAttack);
                        m_LightAttack.reset();
                    }
                    auto newAttack = std::make_shared<LightAttack>(m_Transform.translation);
                    rootRenderer.AddChild(newAttack);
                    m_LightAttack = newAttack;
                    m_LightAttackTimer = 0.0f;
                    m_LightAttackCount++;
                }
                if (m_LightAttack) {
                    m_LightAttack->Update(deltaTime);
                }
                if (m_LightAttackCount >= 4 && m_LightAttackTimer >= 3.0f) {
                    if (m_LightAttack) {
                        rootRenderer.RemoveChild(m_LightAttack);
                        m_LightAttack.reset();
                    }
                    m_CanMoveVertically = true;
                    m_HasStartedLightAttack = false;
                    m_AttackType = AttackType::TYPEB;
                    m_LightAttackCount = 0;
                    m_LightAttackTimer = 0.0f;
                }
                if (!isPlayerDead && m_LightAttack && m_LightAttack->IsActive()) {
                    float laserY = m_LightAttack->GetPosition().y;
                    float boshyY = playerPosition.y;
                    if (fabs(laserY - boshyY) <= 40.0f) {
                        isPlayerDead = true;
                    }
                }
                break;
            }
            case AttackType::TYPEB:
                m_ShootTimerB += deltaTime;
                if (TypeBShootCount < 8 && m_ShootTimerB >= 1.0f) {
                    float randomX = -640.0f + static_cast<float>(rand()) / RAND_MAX * (-100.0f + 640.0f);
                    glm::vec2 spawnPos(randomX, 480.0f);
                    auto bullet = std::make_shared<BulletTypeB>(spawnPos);
                    rootRenderer.AddChild(bullet);
                    m_BulletsB.push_back(bullet);
                    m_ShootTimerB = 0.0f;
                    TypeBShootCount++;
                }
                for (auto& bullet : m_BulletsB) {
                    bullet->Update(deltaTime);
                }
                if (TypeBShootCount >= 8) {
                    int bulletsOutCount = 0;
                    for (auto& bullet : m_BulletsB) {
                        if (bullet->GetPosition().y <= -480.0f) {
                            bulletsOutCount++;
                        }
                    }
                    if (bulletsOutCount >= 6) {
                        m_AttackType = AttackType::TYPEC;
                        TypeBShootCount = 0;
                        ClearGameObjects(m_BulletsB);
                        m_BulletsB.clear();
                    }
                }
                break;
            case AttackType::TYPEC:
                m_ElapsedTimeC += deltaTime;
                m_ShootTimerC += deltaTime;
                for (auto& bullet : m_BulletsC) {
                    bullet->Update(deltaTime);
                }
                if (m_ElapsedTimeC <= TypeCFireDuration) {
                    if (m_ShootTimerC >= TypeCFireInterval) {
                        float waveAngle = 0.0f + 0.5f * sin(TypeC_Frequency * m_ElapsedTimeC);
                        glm::vec2 dir = glm::normalize(glm::vec2(-1.0f, tan(waveAngle)));
                        auto bullet = std::make_shared<BulletTypeC>(m_Transform.translation, dir);
                        rootRenderer.AddChild(bullet);
                        m_BulletsC.push_back(bullet);
                        m_ShootTimerC = 0.0f;
                    }
                }
                m_BulletsC.erase(
                    std::remove_if(m_BulletsC.begin(), m_BulletsC.end(),
                        [](const std::shared_ptr<BulletTypeC>& bullet) {
                            return bullet->GetPosition().x < -700.0f;
                        }),
                    m_BulletsC.end()
                );
                if (m_ElapsedTimeC > TypeCFireDuration && m_BulletsC.empty()) {
                    m_AttackType = AttackType::TYPEA;
                    m_ShootTimerC = 0.0f;
                    m_ElapsedTimeC = 0.0f;
                }
                break;
                default:
                    break;

        }
    }
    m_Transform.translation.x = std::round(m_Transform.translation.x);
    m_Transform.translation.y = std::round(m_Transform.translation.y);

    CheckPlayerDeathByBullets(playerPosition);
}


