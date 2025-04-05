#include "App.hpp"

void App::Update() {
    if (m_GamePhase == GamePhase::MENU) {
        m_MenuSystem->Update();
        if (m_MenuSystem->GetSelectedOption() == Menu::Option::START_GAME) {
            m_GamePhase = GamePhase::WORLD1;
            m_PRM = std::make_shared<ResourceManager>();
            m_PRM->SetPhase(m_World->GetWorldByPhaseName(GamePhaseToString(m_GamePhase))[currentX][currentY]);
            m_Root.AddChild(m_PRM->GetChildren());
        }
    } else if (m_MenuSystem->GetSelectedOption() == Menu::Option::EXIT) {
        m_GamePhase = GamePhase::END;
    }

    static float velocityY = 0;
    const float Gravity = -1;
    const float JumpPower = 15;
    const float MaxFallSpeed = -20;
    static int jumpCount = 0;
    static float shootCooldown = 0;
    const float deltaTime = 1.0f / 60.0f;
    shootCooldown -= deltaTime*4;

    glm::vec2 position = m_Boshy->GetPosition();
    auto* animatedBoshy = dynamic_cast<AnimatedCharacter*>(m_Boshy.get());

    // TileX, TileY 計算與碰撞檢測部分保持不變
    int tileX = static_cast<int>((position.x + 640) / 32);
    int tileY = static_cast<int>((480 - position.y) / 32);

    if (tileX < 0) tileX = 0;
    if (tileX >= m_MapLoader->GetWidth()) tileX = m_MapLoader->GetWidth() - 1;
    if (tileY < 0) tileY = 0;
    if (tileY >= m_MapLoader->GetHeight()) tileY = m_MapLoader->GetHeight() - 1;

    std::cout << "Position: (" << position.x << ", " << position.y << ")"
              << " Tile: (" << tileX << ", " << tileY << ")"
              << " Tile Value: " << m_MapLoader->GetTile(tileX, tileY) << std::endl;

    velocityY += Gravity;
    if (velocityY < MaxFallSpeed) velocityY = MaxFallSpeed;
    position.y += velocityY;

    int belowTile = m_MapLoader->GetTile(tileX, tileY + 1);
    int aboveTile = m_MapLoader->GetTile(tileX, tileY - 1);
    int leftTile = m_MapLoader->GetTile(tileX - 1, tileY);
    int rightTile = m_MapLoader->GetTile(tileX + 1, tileY);

    if (aboveTile == 2 && velocityY > 0) {
        position.y = 480 - ((tileY + 1) * 32);
        velocityY = 0;
    }

    if (belowTile == 1 && velocityY < 0) {
        position.y = 480 - ((tileY) * 32) - 16;
        velocityY = 0;
        jumpCount = 0;
    }

    if (m_MapLoader->GetTile(tileX, tileY) == 1 && velocityY >= 0) {
        position.y = 480 - (tileY - 1) * 32;
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
        if (dir == World::Direction::RIGHT)  ++currentX;
        if (dir == World::Direction::LEFT)   --currentX;
        if (dir == World::Direction::UP)     --currentY;
        if (dir == World::Direction::DOWN)   ++currentY;

        if (m_MapLoader->GetTile(tileX,tileY) == 69) {
            m_PRM->SetPhase(m_World->GetWorldByPhaseName(GamePhaseToString(m_GamePhase))[currentX][currentY]);
            m_MapLoader->LoadMap(m_World->GetWorldByPhaseName(GamePhaseToString(m_GamePhase))[currentX][currentY]);
            position.x *= -1;

        }
        m_Bullets.erase(std::remove_if(m_Bullets.begin(), m_Bullets.end(),
            [](const std::shared_ptr<Bullet>& bullet) { return !bullet->IsVisible(); }),
            m_Bullets.end());
        // 關閉窗口邏輯保持不變
        if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) || Util::Input::IfExit()) {
            m_CurrentState = State::END;
        }
        // 更新角色位置
        m_Boshy->SetPosition(position);

        m_Root.Update();
    }
}
