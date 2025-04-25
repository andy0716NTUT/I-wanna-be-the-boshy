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

void Enemy::Update(float deltaTime, const std::shared_ptr<MapInfoLoader>& mapLoader) {
    if (!m_IsVisible) return;

    if (m_State == MoveState::DEAD) {
        // 處理死亡動畫，然後移除
        m_AttackCooldown += deltaTime;
        if (m_AttackCooldown > 1.0f) {  // 1秒後隱藏
            m_IsVisible = false;
            SetDrawable(nullptr);
        }
        return;
    }

    if (m_State == MoveState::DAMAGED) {
        m_AttackCooldown += deltaTime;
        if (m_AttackCooldown > 0.3f) {  // 受傷狀態持續0.3秒
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
        const float renderOffset = 36.0f; // +32与-4的差值
        const float bearWidth = 66.0f;    // 熊的宽度
        const float bearHeight = 70.0f;   // 熊的高度

        // 获取用于碰撞检测的位置
        float collisionY = position.y - renderOffset;

        // 获取熊当前位置对应的瓦片坐标（用碰撞位置计算）
        int tileX = static_cast<int>((position.x + 640) / 16);
        int tileY = static_cast<int>((480 - collisionY) / 16);

        // 检查左、中、右三个点的下方瓦片
        int leftTileX = static_cast<int>((position.x - bearWidth/3 + 640) / 16);
        int centerTileX = tileX;
        int rightTileX = static_cast<int>((position.x + bearWidth/3 + 640) / 16);

        int belowLeftTile = mapLoader->GetTile(leftTileX, tileY + 1);
        int belowCenterTile = mapLoader->GetTile(centerTileX, tileY + 1);
        int belowRightTile = mapLoader->GetTile(rightTileX, tileY + 1);

        // 任一点下方为地面即视为站在地面上
        bool isOnGround = (m_VelocityY < 0) &&
                         ((belowLeftTile == 1 || belowLeftTile == 8) ||
                          (belowCenterTile == 1 || belowCenterTile == 8) ||
                          (belowRightTile == 1 || belowRightTile == 8));

        if (isOnGround) {
            // 使用新的偏移值计算渲染位置
            position.y = 480 - ((tileY) * 16) + 22;  // 精确设置地面高度
            m_VelocityY = 0;
            m_IsOnGround = true;
        } else {
            m_IsOnGround = false;
        }

        // 在地面上才进行左右移动
        if (m_IsOnGround) {
            if (m_Direction == Character::direction::RIGHT) {
                // 向右移动代码...
                position.x += m_Speed;

                // 计算熊前方(右侧)的瓦片坐标
                int frontTileX = static_cast<int>((position.x + bearWidth/2 + 640) / 16);
                int frontTileY = tileY;

                // 检查多个高度点的墙壁
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

                // 如果碰到墙壁或前方无地面，改变方向
                if (hasWallRight || !hasGround) {
                    m_Direction = Character::direction::LEFT;
                    SetState(MoveState::WALK_LEFT);
                }
            } else {
                // 向左移动代码...
                position.x -= m_Speed;

                // 计算熊前方(左侧)的瓦片坐标
                int frontTileX = static_cast<int>((position.x - bearWidth/2 + 640) / 16);
                int frontTileY = tileY;

                // 检查多个高度点的墙壁
                bool hasWallLeft = false;
                for (int yOffset = 0; yOffset < 2; yOffset++) {
                    int frontTile = mapLoader->GetTile(frontTileX, frontTileY - yOffset);
                    if (frontTile == 2) {
                        hasWallLeft = true;
                        break;
                    }
                }

                // 检查前方地面
                int groundTile = mapLoader->GetTile(frontTileX, tileY + 1);
                bool hasGround = (groundTile == 1 || groundTile == 8);

                // 如果碰到墙壁或前方无地面，改变方向
                if (hasWallLeft || !hasGround) {
                    m_Direction = Character::direction::RIGHT;
                    SetState(MoveState::WALK);
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

    // 假設地圖中使用特定ID代表不同類型的敵人
    const int ENEMY_BASIC_ID = 20;
    const int ENEMY_FLYING_ID = 21;
    const int ENEMY_SHOOTER_ID = 22;
    const int BEAR_ID = 23;  // 熊的ID

    // 檢查Map4中的特定位置[55,39]
    if (mapLoader->GetCurrentPhase() == "4" ||
        mapLoader->GetCurrentPhase() == "4_1" ||
        mapLoader->GetCurrentPhase() == "4_2") {

        // 在位置[55,39]創建熊敵人
        float posX = 55 * 16 - 640;
        float posY = 480 - 34 * 16;

        auto bear = CreateEnemy({posX, posY}, EnemyType::BASIC, renderer);
        bear->SetSpeed(2.0f);  // 設置移動速度
        bear->SetHealth(3);    // 設置生命值
        enemies.push_back(bear);
        }

    // 掃描地圖尋找其他敵人
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
                type = EnemyType::BASIC;  // 使用基本類型表示熊
            } else {
                continue;
            }

            float posX = x * 16 - 640;
            float posY = 480 - y * 16;

            auto enemy = CreateEnemy({posX, posY}, type, renderer);
            enemies.push_back(enemy);

            // 清除地圖中的敵人標記，避免重複生成
            mapLoader->SetTile(x, y, 0);
        }
    }

    return enemies;
}