#include "App.hpp"

void App::Update() {
    static float velocityY = 0;
    const float Gravity = -1;
    const float JumpPower = 15;
    const float MaxFallSpeed = -10;
    static int jumpCount = 0;
    static float shootCooldown = 0;
    const float deltaTime = 1.0f / 60.0f;
    static float switchTimer = 0.0f; // 使用浮點數計時器（秒）
    static bool isSwitch = false;     // 切換狀態
    const float switchInterval = 1.0f; // 每3秒切換一次
    shootCooldown -= deltaTime*4;
    glm::vec2 position = m_Boshy->GetPosition();
    auto* animatedBoshy = dynamic_cast<AnimatedCharacter*>(m_Boshy.get());



    // TileX, TileY 計算與碰撞檢測部分保持不變
    int tileX = static_cast<int>((position.x + 640) / 16);
    int tileY = static_cast<int>((480 - position.y) / 16);

    if (tileX < 0) tileX = 0;
    if (tileX >= m_MapLoader->GetWidth()) tileX = m_MapLoader->GetWidth() - 1;
    if (tileY < 0) tileY = 0;
    if (tileY >= m_MapLoader->GetHeight()) tileY = m_MapLoader->GetHeight() - 1;

    std::cout << "Position: (" << currentX << ", " << currentY << ")"
              << " Tile: (" << tileX << ", " << tileY << ")"
              << " Tile Value: " << m_MapLoader->GetTile(tileX, tileY) << std::endl;

    velocityY += Gravity;
    if (velocityY < MaxFallSpeed) velocityY = MaxFallSpeed;
    position.y += velocityY;

    int belowTile = m_MapLoader->GetTile(tileX, tileY + 1);
    int aboveTile = m_MapLoader->GetTile(tileX, tileY - 1);
    int leftTile = m_MapLoader->GetTile(tileX - 1, tileY);
    int rightTile = m_MapLoader->GetTile(tileX + 1, tileY);

    if ((aboveTile == 2 || aboveTile == 1)&& velocityY > 0) {
        position.y = 480 - ((tileY + 1) * 16);
        velocityY = 0;
    }

    if (belowTile == 1 && velocityY < 0) {
        position.y = 480 - ((tileY) * 16) - 4;
        velocityY = 0;
        jumpCount = 0;
    }

    if (m_MapLoader->GetTile(tileX, tileY) == 1 && velocityY >= 0) {
        position.y = 480 - (tileY - 1) * 16;
        velocityY = 0;
    }

    if (Util::Input::IsKeyPressed(Util::Keycode::RIGHT)) {
        float prevX = position.x;
        if (rightTile != 2) {
            position.x += 5;
            animatedBoshy->SetDirection(Character::direction::RIGHT);
        }
        if (position.x != prevX) {
            animatedBoshy->SetState(Character::MoveState::RUN);
        } else {
            animatedBoshy->SetState(Character::MoveState::IDLE);
        }
    }
    if (Util::Input::IsKeyPressed(Util::Keycode::LEFT)) {
        float prevX = position.x;
        if (leftTile != 2) {
            position.x -= 5;
            animatedBoshy->SetDirection(Character::direction::LEFT);
        }
        if (position.x != prevX) {
            animatedBoshy->SetState(Character::MoveState::RUN_LEFT);
        } else {
            animatedBoshy->SetState(Character::MoveState::IDLE_LEFT);
        }
    }
    shootCooldown -= deltaTime;
    // 射擊邏輯保持不變
    if (shootCooldown <= 0 && Util::Input::IsKeyPressed(Util::Keycode::X) && m_Bullets.size() < 5) {
        auto bullet = std::make_shared<Bullet>();
        bullet->SetPosition(m_Boshy->GetPosition());
        bullet->SetLifeTime(2.0f);
        bullet->SetVisible(true);
        if (m_Boshy->GetDirection() == Character::direction::LEFT) {
            bullet->SetImage(RESOURCE_DIR"/Image/bullet2.png");
            bullet->SetDirection(Character::direction::LEFT);
        } else {
            bullet->SetImage(RESOURCE_DIR"/Image/bullet.png");
            bullet->SetDirection(Character::direction::RIGHT);
        }
        m_Bullets.push_back(bullet);
        m_Root.AddChild(bullet);
        shootCooldown = 0.5f;
    }

    else if (Util::Input::IsKeyDown(Util::Keycode::Z) && jumpCount < 2) {
        velocityY = JumpPower;
        jumpCount++;
        if (animatedBoshy->GetDirection() == Character::direction::LEFT) {
            animatedBoshy->SetState(Character::MoveState::JUMP_LEFT);
        } else {
            animatedBoshy->SetState(Character::MoveState::JUMP);
        }
    } else if (animatedBoshy->IfAnimationEnds() || (animatedBoshy->GetState() != Character::MoveState::JUMP && velocityY == 0)) {
        if (animatedBoshy->GetDirection() == Character::direction::LEFT) {
            animatedBoshy->SetState(Character::MoveState::IDLE_LEFT);
        } else {
            animatedBoshy->SetState(Character::MoveState::IDLE);
        }
    }

    // 子彈移動邏輯修正
    m_Bullet->Update(deltaTime); // 先更新子彈內部狀態（例如生命週期）
    if (m_Bullet->IsVisible()) {
        glm::vec2 bulletPosition = m_Bullet->GetPosition();
        if (m_Bullet->GetDirection() == Character::direction::LEFT) {
            bulletPosition.x -= 10.0f; // Move left with speed 5.0f
        } else {
            bulletPosition.x += 10.0f; // Move right with speed 5.0f
        }
        m_Bullet->SetPosition(bulletPosition);
    }
    for (auto& bullet : m_Bullets) {
        bullet->Update(deltaTime);
        if (bullet->IsVisible()) {
            glm::vec2 bulletPosition = bullet->GetPosition();
            if (bullet->GetDirection() == Character::direction::LEFT) {
                bulletPosition.x -= 10.0f;
            } else {
                bulletPosition.x += 10.0f;
            }
            bullet->SetPosition(bulletPosition);
        }
    }
    int tile = m_MapLoader->GetTile(tileX, tileY);
    World::Direction dir = m_World->GetTeleportDirection(tile);

    if (dir != World::Direction::NONE) {
        if (dir == World::Direction::RIGHT) {
            ++currentY;
            position.x *= -1;
            position.x += 32;
        };
        if (dir == World::Direction::LEFT) {
            --currentY;
            position.x *= -1;
            position.x -= 32;
        }
        if (dir == World::Direction::UP)     --currentX;
        if (dir == World::Direction::DOWN)   ++currentX;

        CurrentPhase = m_World->GetWorldByPhaseName(GamePhaseToString(m_GamePhase))[currentX][currentY];
        for (auto& bullet : m_Bullets) {
            bullet->SetVisible(false);
            bullet->SetDrawable(nullptr); // 清除圖片資源
        }
        m_Bullets.clear();
        // 如果不是 phase 3，就直接切換
        if (CurrentPhase != "3") {
            m_PRM->SetPhase(CurrentPhase);
            m_MapLoader->LoadMap(CurrentPhase);
        }
        if (CurrentPhase == "3") {
            m_PRM->SetPhase("3_1");
            m_MapLoader->LoadMap("3_1");
        }

        m_CheckPoints.clear();
        m_CheckPoints = CheckPoint::CreateFromMap(m_MapLoader, m_Root);
        std::cout << "Current Phase : " << CurrentPhase << std::endl;
    }

    if (CurrentPhase == "3") {
        switchTimer += deltaTime;
        std::cout << switchTimer << std::endl;
        if (switchTimer >= switchInterval) {
            isSwitch = !isSwitch;
            std::string newPhase = isSwitch ? "3_2" : "3_1";
            m_PRM->SetPhase(newPhase);
            m_MapLoader->LoadMap(newPhase);
            switchTimer = 0.0f; // 重置計時器
        }
    }
    for (auto& checkpoint : m_CheckPoints) {
        glm::vec2 cpPos = checkpoint->GetPosition();
        for (auto& bullet : m_Bullets) {
            glm::vec2 bulletPos = bullet->GetPosition();
            if (glm::distance(cpPos, bulletPos) < 20.0f) {
                checkpoint->play();
                bullet->SetVisible(false);
                currentCheckPoint = m_Boshy->GetPosition();
            }
        }
    }

    // 清除不可見的子彈
    Bullet::CleanBullet(m_Bullets);

    // 關閉或重生邏輯
    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) || Util::Input::IfExit()) {
        m_CurrentState = State::END;
    }

    if (Util::Input::IsKeyDown(Util::Keycode::R)) {
        position = currentCheckPoint;
    }

    // 更新角色位置與整體狀態
    m_Boshy->SetPosition(position);
    m_Root.Update();
}

