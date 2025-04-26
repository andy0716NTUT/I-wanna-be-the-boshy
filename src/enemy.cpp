#include "enemy.hpp"

void Enemy::SetPosition(const glm::vec2& position) {
    m_Transform.translation = position;
    if (m_State == MoveState::IDLE) {
        m_StartPosition = position;  // 記錄起始位置用於巡邏
    }
}

void Enemy::SetState(MoveState state) {
    if (m_State == state) return;

    m_State = state;
    if (m_Animations.find(state) != m_Animations.end()) {
        SetDrawable(m_Animations[state]);
    }
}

bool Enemy::TakeDamage(int damage) {
    m_Health -= damage;
    if (m_Health <= 0) {
        SetState(MoveState::DEAD);
        return true;  // 敵人已死亡
    }

    SetState(MoveState::DAMAGED);
    return false;  // 敵人還活著
}

void Enemy::Update(float deltaTime, const std::shared_ptr<MapInfoLoader>& mapLoader, const glm::vec2& playerPos) {
    if (!m_IsVisible) return;

    // 保存玩家位置用于追踪
    m_PlayerPos = playerPos;

    if (m_State == MoveState::DEAD) {
        // 处理死亡动画，然后移除
        m_AttackCooldown += deltaTime;
        if (m_AttackCooldown > 1.0f) {
            m_IsVisible = false;
            SetDrawable(nullptr);
        }
        return;
    }

    if (m_State == MoveState::DAMAGED) {
        m_AttackCooldown += deltaTime;
        if (m_AttackCooldown > 0.3f) {
            m_AttackCooldown = 0.0f;
            SetState(MoveState::IDLE);
        }
        return;
    }

    UpdateMovement(deltaTime, mapLoader);
}

void Enemy::UpdateMovement(float deltaTime, const std::shared_ptr<MapInfoLoader>& mapLoader) {
    glm::vec2 position = GetPosition();

    if (m_Type == EnemyType::BASIC) {
        // 应用重力
        m_VelocityY += m_Gravity;
        if (m_VelocityY < m_MaxFallSpeed) m_VelocityY = m_MaxFallSpeed;
        position.y += m_VelocityY;

        // 渲染与碰撞位置分离处理
        const float renderOffset = 36.0f;
        const float bearWidth = 66.0f;
        const float bearHeight = 70.0f;

        // 获取用于碰撞检测的位置
        float collisionY = position.y - renderOffset;

        // 获取熊当前位置对应的瓦片坐标
        int tileX = static_cast<int>((position.x + 640) / 16);
        int tileY = static_cast<int>((480 - collisionY) / 16);

        // 检查左中右三点的下方瓦片
        int leftTileX = static_cast<int>((position.x - bearWidth/3 + 640) / 16);
        int centerTileX = tileX;
        int rightTileX = static_cast<int>((position.x + bearWidth/3 + 640) / 16);

        int belowLeftTile = mapLoader->GetTile(leftTileX, tileY + 1);
        int belowCenterTile = mapLoader->GetTile(centerTileX, tileY + 1);
        int belowRightTile = mapLoader->GetTile(rightTileX, tileY + 1);

        // 检查是否在地面上
        bool isOnGround = (m_VelocityY < 0) &&
                         ((belowLeftTile == 1 || belowLeftTile == 8) ||
                          (belowCenterTile == 1 || belowCenterTile == 8) ||
                          (belowRightTile == 1 || belowRightTile == 8));

        if (isOnGround) {
            position.y = 480 - ((tileY) * 16) + 22;
            m_VelocityY = 0;
            m_IsOnGround = true;
        } else {
            m_IsOnGround = false;
        }

        // 在地面上才进行追踪玩家
        if (m_IsOnGround && glm::length(m_PlayerPos) > 0) {
            // 判断玩家在熊的哪一侧
            bool playerIsRight = m_PlayerPos.x > position.x;

            // 更新目标方向，但不立即改变实际方向
            Character::direction newTargetDir = playerIsRight ?
                Character::direction::RIGHT : Character::direction::LEFT;

            // 检测目标方向是否变化
            if (newTargetDir != m_TargetDirection) {
                m_TargetDirection = newTargetDir;
                m_TurnDelayTimer = 0.0f; // 重置计时器
            }

            // 增加延迟计时器
            m_TurnDelayTimer += deltaTime;

            // 只有当延迟时间达到后，才实际改变方向
            if (m_TurnDelayTimer >= m_DirectionChangeDelay) {
                // 检测是否需要真正转向
                if (m_Direction != m_TargetDirection) {
                    // 转向时触发减速效果
                    m_HitWall = true;
                    m_CurrentSpeed = m_MaxSpeed * 0.3f; // 减速到30%
                    m_RecoveryTimer = 0.0f;

                    // 实际改变方向
                    m_Direction = m_TargetDirection;
                }
            }

            // 根据当前方向移动熊
            if (m_Direction == Character::direction::RIGHT) {
                SetState(MoveState::WALK);

                // 向右移动前检查墙壁
                int frontTileX = static_cast<int>((position.x + bearWidth/2 + 640) / 16);
                int frontTileY = tileY;

                bool hasWallRight = false;
                for (int yOffset = 0; yOffset < 2; yOffset++) {
                    int frontTile = mapLoader->GetTile(frontTileX, frontTileY - yOffset);
                    if (frontTile == 2) {
                        hasWallRight = true;
                        break;
                    }
                }

                // 检查前方地面
                int groundTile = mapLoader->GetTile(frontTileX, tileY + 1);
                bool hasGround = (groundTile == 1 || groundTile == 8);

                if (hasWallRight) {
                    // 撞墙时速度减少
                    if (!m_HitWall) {
                        m_HitWall = true;
                        m_CurrentSpeed = m_MaxSpeed * 0.3f;
                        m_RecoveryTimer = 0.0f;
                    }
                } else if (!hasGround) {
                    // 悬崖边缘转向
                    m_Direction = Character::direction::LEFT;
                    SetState(MoveState::WALK_LEFT);
                } else {
                    // 未撞墙时逐渐加速
                    if (m_HitWall) {
                        m_RecoveryTimer += deltaTime;
                        if (m_RecoveryTimer > 0.5f) {
                            // 0.5秒后开始恢复速度
                            m_CurrentSpeed += m_Acceleration * m_MaxSpeed * deltaTime * 10;
                            if (m_CurrentSpeed >= m_MaxSpeed) {
                                m_CurrentSpeed = m_MaxSpeed;
                                m_HitWall = false;
                            }
                        }
                    }

                    // 使用当前速度移动，确保平滑
                    position.x += m_CurrentSpeed * deltaTime * 60;
                }
            } else {
                // 向左移动逻辑，与向右类似
                SetState(MoveState::WALK_LEFT);

                int frontTileX = static_cast<int>((position.x - bearWidth/2 + 640) / 16);
                int frontTileY = tileY;

                bool hasWallLeft = false;
                for (int yOffset = 0; yOffset < 2; yOffset++) {
                    int frontTile = mapLoader->GetTile(frontTileX, frontTileY - yOffset);
                    if (frontTile == 2) {
                        hasWallLeft = true;
                        break;
                    }
                }

                int groundTile = mapLoader->GetTile(frontTileX, tileY + 1);
                bool hasGround = (groundTile == 1 || groundTile == 8);

                if (hasWallLeft) {
                    if (!m_HitWall) {
                        m_HitWall = true;
                        m_CurrentSpeed = m_MaxSpeed * 0.3f;
                        m_RecoveryTimer = 0.0f;
                    }
                } else if (!hasGround) {
                    m_Direction = Character::direction::RIGHT;
                    SetState(MoveState::WALK);
                } else {
                    if (m_HitWall) {
                        m_RecoveryTimer += deltaTime;
                        if (m_RecoveryTimer > 0.5f) {
                            m_CurrentSpeed += m_Acceleration * m_MaxSpeed * deltaTime * 10;
                            if (m_CurrentSpeed >= m_MaxSpeed) {
                                m_CurrentSpeed = m_MaxSpeed;
                                m_HitWall = false;
                            }
                        }
                    }

                    // 向左移动，考虑deltaTime以确保一致性
                    position.x -= m_CurrentSpeed * deltaTime * 60;
                }
            }
        }
    }

    SetPosition(position);
}
bool Enemy::IsValidPosition(const glm::vec2& position, const std::shared_ptr<MapInfoLoader>& mapLoader) {
    int tileX = static_cast<int>((position.x + 640) / 16);
    int tileY = static_cast<int>((480 - position.y) / 16);

    if (tileX < 0 || tileX >= mapLoader->GetWidth() ||
        tileY < 0 || tileY >= mapLoader->GetHeight()) {
        return false;
    }

    int tileValue = mapLoader->GetTile(tileX, tileY);
    return tileValue != 1 && tileValue != 2;  // 不是牆壁和障礙物
}

bool Enemy::CheckBulletCollision(const std::vector<std::shared_ptr<Bullet>>& bullets) {
    // 如果是熊类型敌人，直接返回false表示没有碰撞
    if (m_Type == EnemyType::BASIC) return false;

    if (!m_IsVisible || m_State == MoveState::DEAD) return false;

    glm::vec2 enemyPos = GetPosition();
    float enemyWidth = 32.0f;
    float enemyHeight = 32.0f;

    for (const auto& bullet : bullets) {
        if (!bullet->IsVisible()) continue;

        glm::vec2 bulletPos = bullet->GetPosition();
        float bulletWidth = 16.0f;
        float bulletHeight = 16.0f;

        // 检查碰撞
        bool collision = std::abs(bulletPos.x - enemyPos.x) < (enemyWidth / 2 + bulletWidth / 2) &&
                        std::abs(bulletPos.y - enemyPos.y) < (enemyHeight / 2 + bulletHeight / 2);

        if (collision) {
            // 子弹命中后消失
            bullet->SetVisible(false);
            bullet->SetDrawable(nullptr);
            return true;
        }
    }

    return false;
}

bool Enemy::CheckPlayerCollision(const glm::vec2& playerPos) {
    const float bearWidth = 60.0f;  // 熊的寬度
    const float bearHeight = 70.0f; // 熊的高度
    const float playerWidth = 16.0f; // 玩家寬度
    const float playerHeight = 16.0f; // 玩家高度

    // 計算熊的邊界
    float bearLeft = m_Transform.translation.x - bearWidth / 2;
    float bearRight = m_Transform.translation.x + bearWidth / 2;
    float bearTop = m_Transform.translation.y + bearHeight / 2;
    float bearBottom = m_Transform.translation.y - bearHeight / 2;

    // 計算玩家的邊界
    float playerLeft = playerPos.x - playerWidth / 2;
    float playerRight = playerPos.x + playerWidth / 2;
    float playerTop = playerPos.y + playerHeight / 2;
    float playerBottom = playerPos.y - playerHeight / 2;

    // 檢測是否有重疊
    return bearRight > playerLeft && bearLeft < playerRight &&
           bearTop > playerBottom && bearBottom < playerTop;
}

void Enemy::LoadAnimations(EnemyType type) {
    std::vector<std::string> idleFrames;
    std::vector<std::string> walkFrames;
    std::vector<std::string> attackFrames;
    std::vector<std::string> damagedFrames;
    std::vector<std::string> deadFrames;

    switch (type) {
        case EnemyType::BASIC:
            // 熊的動畫資源
            idleFrames = {RESOURCE_DIR "/Image/enemy/bear1.png"};
            walkFrames = {RESOURCE_DIR "/Image/enemy/bear1.png", RESOURCE_DIR "/Image/enemy/bear2.png"}; // 左走
            m_Animations[MoveState::WALK_LEFT] = std::make_shared<Util::Animation>(walkFrames, true, 200, true);
            m_Drawable = m_Animations[MoveState::IDLE];
            walkFrames = {RESOURCE_DIR "/Image/enemy/bear3.png", RESOURCE_DIR "/Image/enemy/bear4.png"}; // 右走
            m_Animations[MoveState::WALK] = std::make_shared<Util::Animation>(walkFrames, true, 200, true);
            m_Drawable = m_Animations[MoveState::IDLE];
            damagedFrames = {RESOURCE_DIR "/Image/bearHurt.png"};
            deadFrames = {RESOURCE_DIR "/Image/bearDead.png"};
            break;

        case EnemyType::FLYING:
            // 飛行敵人的動畫
            idleFrames = {RESOURCE_DIR "/Image/enemy1.png"};
            walkFrames = {RESOURCE_DIR "/Image/enemy1.png", RESOURCE_DIR "/Image/enemy2.png"};
            break;

        case EnemyType::SHOOTER:
            // 射擊敵人的動畫
            idleFrames = {RESOURCE_DIR "/Image/enemy3.png"};
            attackFrames = {RESOURCE_DIR "/Image/enemy3.png", RESOURCE_DIR "/Image/enemy4.png"};
            break;
    }

    m_Animations[MoveState::IDLE] = std::make_shared<Util::Animation>(idleFrames, true, 200, true);

    if (!walkFrames.empty() && type != EnemyType::BASIC) // 除熊外的普通走路動畫
        m_Animations[MoveState::WALK] = std::make_shared<Util::Animation>(walkFrames, true, 200, true);

    if (!attackFrames.empty())
        m_Animations[MoveState::ATTACK] = std::make_shared<Util::Animation>(attackFrames, true, 200, false);

    if (!damagedFrames.empty())
        m_Animations[MoveState::DAMAGED] = std::make_shared<Util::Animation>(damagedFrames, true, 300, false);

    if (!deadFrames.empty())
        m_Animations[MoveState::DEAD] = std::make_shared<Util::Animation>(deadFrames, true, 300, false);

    // 設置預設動畫
    SetDrawable(m_Animations[MoveState::IDLE]);
}

std::shared_ptr<Enemy> Enemy::CreateEnemy(
    const glm::vec2& position,
    EnemyType type,
    Util::Renderer& renderer) {

    auto enemy = std::make_shared<Enemy>();
    enemy->m_Type = type;
    enemy->SetPosition(position);
    enemy->LoadAnimations(type);

    // 根據類型設置屬性
    switch (type) {
        case EnemyType::BASIC:
            enemy->m_Health = 2;
            enemy->m_Speed = 2.0f;
            break;

        case EnemyType::FLYING:
            enemy->m_Health = 1;
            enemy->m_Speed = 3.0f;
            break;

        case EnemyType::SHOOTER:
            enemy->m_Health = 3;
            enemy->m_Speed = 0.0f; // 射擊敵人通常不移動
            break;
    }

    renderer.AddChild(enemy);
    return enemy;
}

std::vector<std::shared_ptr<Enemy>> Enemy::CreateFromMap(
    const std::shared_ptr<MapInfoLoader>& mapLoader,
    Util::Renderer& renderer) {

    std::vector<std::shared_ptr<Enemy>> enemies;
    std::string currentPhase = mapLoader->GetCurrentPhase();

    // 定义敌人ID
    const int ENEMY_BASIC_ID = 20;
    const int ENEMY_FLYING_ID = 21;
    const int ENEMY_SHOOTER_ID = 22;
    const int BEAR_ID = 23;

    // 只在地图4系列创建熊敌人
    if (currentPhase == "4" || currentPhase == "4_1" || currentPhase == "4_2") {
        float posX = 55 * 16 - 640;
        float posY = 480 - 34 * 16;

        auto bear = CreateEnemy({posX, posY}, EnemyType::BASIC, renderer);
        bear->SetSpeed(10.0f);
        bear->SetHealth(3);
        enemies.push_back(bear);
    }

    // 从地图中加载其他敌人（根据地图ID决定）
    // 可以根据不同地图添加条件，例如：if (currentPhase != "1" && currentPhase != "2")
    for (int y = 0; y < mapLoader->GetHeight(); ++y) {
        for (int x = 0; x < mapLoader->GetWidth(); ++x) {
            int tileValue = mapLoader->GetTile(x, y);
            EnemyType type;

            if (tileValue == ENEMY_BASIC_ID) {
                type = EnemyType::BASIC;
            } else if (tileValue == ENEMY_FLYING_ID) {
                type = EnemyType::FLYING;
            } else if (tileValue == ENEMY_SHOOTER_ID) {
                type = EnemyType::SHOOTER;
            } else if (tileValue == BEAR_ID) {
                // 跳过在地图中标记的熊，因为我们已经在特定位置创建了熊
                if (currentPhase == "4" || currentPhase == "4_1" || currentPhase == "4_2") {
                    mapLoader->SetTile(x, y, 0);
                    continue;
                }
                type = EnemyType::BASIC;
            } else {
                continue;
            }

            float posX = x * 16 - 640;
            float posY = 480 - y * 16;

            auto enemy = CreateEnemy({posX, posY}, type, renderer);
            enemies.push_back(enemy);
            mapLoader->SetTile(x, y, 0);
        }
    }

    return enemies;
}
void Enemy::SetSpeed(float speed) {
    m_Speed = speed;
    m_MaxSpeed = speed;
    m_CurrentSpeed = speed;
}