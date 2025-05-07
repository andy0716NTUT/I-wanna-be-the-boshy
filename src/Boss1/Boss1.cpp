#include "../include/Boss1/Boss1.hpp"

#include "GameObjectHelper.hpp"

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
        if (m_CanMoveVertically) {  // ⭐ 只有允許時才上下移動
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

                // 更新所有子彈
                for (auto& bullet : m_BulletsA) {
                    bullet->Update(deltaTime);
                    if (glm::distance(bullet->GetPosition(),playerPosition) < 20.0f) {
                        //this->isPlayerDead = true;
                    }
                }
                // 檢查是否所有子彈都飛出場外
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
                        ClearGameObjects(m_BulletsA);// 清空子彈列表（可選）
                    }
                }
                    break;
            case AttackType::LIGHTATTACK: {
                if (!m_HasStartedLightAttack) {
                    m_CanMoveVertically = false;  // 停掉 sin()
                    m_HasStartedLightAttack = true;
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
                    // ⭐ 每次發射前 → 抓最新 playerPosition.y
                    m_CanMoveVertically = true;
                    m_Transform.translation.y = playerPosition.y;

                    // ⭐ 清掉舊的
                    if (m_LightAttack) {
                        rootRenderer.RemoveChild(m_LightAttack);
                        m_LightAttack.reset();
                    }

                    // ⭐ 建立新光束
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
                }

                break;
            }
            case AttackType::TYPEB:
                m_ShootTimerB += deltaTime;

                // 每 0.5 秒發射一發
                if (TypeBShootCount < 8 && m_ShootTimerB >= 1.0f) {
                    float randomX = -640.0f + static_cast<float>(rand()) / RAND_MAX * (-100.0f + 640.0f);
                    glm::vec2 spawnPos(randomX, 480.0f); // 假設畫面頂端 y=480
                    auto bullet = std::make_shared<BulletTypeB>(spawnPos);
                    rootRenderer.AddChild(bullet);
                    m_BulletsB.push_back(bullet);

                    m_ShootTimerB = 0.0f;
                    TypeBShootCount++;
                }

                // 更新所有 BulletTypeB
                for (auto& bullet : m_BulletsB) {
                    bullet->Update(deltaTime);
                }

                // 判斷是否所有子彈都掉到螢幕外（或某個 y 座標以下）
                if (TypeBShootCount >= 8) {
                    bool allBulletsOut = true;
                    for (auto& bullet : m_BulletsB) {
                        if (bullet->GetPosition().y >= -480.0f) {  // 假設螢幕高度 480
                            allBulletsOut = false;
                            break;
                        }
                    }
                    if (allBulletsOut) {
                        m_AttackType = AttackType::TYPEC;
                        ClearGameObjects(m_BulletsB);
                        m_BulletsB.clear();
                    }
                }
                break;
            case AttackType::TYPEC:
                m_ElapsedTimeC += deltaTime;
                m_ShootTimerC += deltaTime;

            if (m_ElapsedTimeC <= TypeCFireDuration) {
                if (m_ShootTimerC >= TypeCFireInterval) {
                    // 計算發射角度（用 sin 調整）
                    float waveAngle = 0.0f + 0.5f * sin(TypeC_Frequency * m_ElapsedTimeC); // 調整角度幅度
                    glm::vec2 dir = glm::normalize(glm::vec2(-1.0f, tan(waveAngle)));  // 左偏一點上下波浪

                    auto bullet = std::make_shared<BulletTypeC>(m_Transform.translation, dir);
                    rootRenderer.AddChild(bullet);
                    m_BulletsC.push_back(bullet);

                    m_ShootTimerC = 0.0f;
                }
            }

            // 更新子彈

            // 清除飛出螢幕的子彈
            m_BulletsC.erase(
                std::remove_if(m_BulletsC.begin(), m_BulletsC.end(),
                    [](const std::shared_ptr<BulletTypeC>& bullet) {
                        return bullet->GetPosition().x < -700.0f;  // 已經飛出螢幕
                    }),
                m_BulletsC.end()
            );

            if ( m_BulletsC.empty()) {
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
}


