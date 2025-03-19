#include "App.hpp"

void App::Update() {
    if (m_GamePhase == GamePhase::MENU) {
        m_MenuSystem->Update();
        if (m_MenuSystem->GetSelectedOption() == Menu::Option::START_GAME) {
            m_GamePhase = GamePhase::WORLD1;
            m_PRM = std::make_shared<ResourceManager>();
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
    const float BulletSpeed = 5.0f;
    const float deltaTime = 1.0f / 60.0f;
    shootCooldown -= deltaTime;

    glm::vec2 position = m_Boshy->GetPosition();
    auto* animatedBoshy = dynamic_cast<AnimatedCharacter*>(m_Boshy.get());

    // **修正 TileX, TileY 計算**
    int tileX = static_cast<int>((position.x + 640) / 32);
    int tileY = static_cast<int>((384 - position.y) / 32);

    // **確保 TileX, TileY 不超出範圍**
    if (tileX < 0) tileX = 0;
    if (tileX >= m_MapLoader->GetWidth()) tileX = m_MapLoader->GetWidth() - 1;
    if (tileY < 0) tileY = 0;
    if (tileY >= m_MapLoader->GetHeight()) tileY = m_MapLoader->GetHeight() - 1;

    // **Debug 訊息**
    std::cout << "Position: (" << position.x << ", " << position.y << ")"
              << " Tile: (" << tileX << ", " << tileY << ")"
              << " Tile Value: " << m_MapLoader->GetTile(tileX, tileY) << std::endl;

    // **施加重力**
    velocityY += Gravity;
    if (velocityY < MaxFallSpeed) velocityY = MaxFallSpeed;
    position.y += velocityY;

    // **取得角色四周的 Tile**
    int belowTile = m_MapLoader->GetTile(tileX, tileY + 1);  // 角色腳下
    int aboveTile = m_MapLoader->GetTile(tileX, tileY - 1);  // 角色頭頂
    int leftTile = m_MapLoader->GetTile(tileX - 1, tileY);   // 左邊
    int rightTile = m_MapLoader->GetTile(tileX + 1, tileY);  // 右邊

    // **防止角色從上方穿透 `2`（天花板）**
    if (aboveTile == 2 && velocityY > 0) {
        position.y = 384 - ((tileY + 1) * 32); // 讓角色頂住 `2`，不能再上升
        velocityY = 0;
    }

    // **地板碰撞檢測**
    if (belowTile == 1 && velocityY < 0) {
        position.y = 384 - ((tileY) * 32) - 16; // **讓角色對齊地板**
        velocityY = 0;
        jumpCount = 0;
    }

    // **防止角色從側面或底部穿入 `1`**
    if (m_MapLoader->GetTile(tileX, tileY) == 1 && velocityY >= 0) {
        position.y = 384 - (tileY - 1) * 32;
        velocityY = 0;
    }

    // **牆壁碰撞（不可穿透 `2`）**
    if (Util::Input::IsKeyPressed(Util::Keycode::RIGHT)) {
        float prevX = position.x;  // 記錄變更前的 X 位置
        if (rightTile != 2) {
            position.x += 5;
            animatedBoshy->SetDirection(Character::direction::RIGHT);
        }
        // **如果 X 位置沒變，就不要設為 RUN**
        if (position.x != prevX) {
            animatedBoshy->SetState(Character::MoveState::RUN);
        } else {
            animatedBoshy->SetState(Character::MoveState::IDLE);
        }
    }
    if (Util::Input::IsKeyPressed(Util::Keycode::LEFT)) {
        float prevX = position.x;  // 記錄變更前的 X 位置
        if (leftTile != 2) {
            position.x -= 5;
            animatedBoshy->SetDirection(Character::direction::LEFT);
        }
        // **如果 X 位置沒變，就不要設為 RUN_LEFT**
        if (
position.x != prevX) {
            animatedBoshy->SetState(Character::MoveState::RUN_LEFT);
        } else {
            animatedBoshy->SetState(Character::MoveState::IDLE_LEFT);
        }
    }

    // **角色狀態變更**
    if (shootCooldown <= 0 && Util::Input::IsKeyDown(Util::Keycode::X)) {
        if (m_Boshy->GetDirection() == Character::direction::LEFT) {
            animatedBoshy->SetState(Character::MoveState::SHOOT_LEFT);
        } else {
            animatedBoshy->SetState(Character::MoveState::SHOOT);
        }
        m_Bullet->SetPosition(m_Boshy->GetPosition() + glm::vec2{10, 0});
        m_Bullet->SetVisible(true);
        shootCooldown = 0.5f;
    } else if (Util::Input::IsKeyDown(Util::Keycode::Z) && jumpCount < 2) {
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

    // **更新角色位置**
    m_Boshy->SetPosition(position);
    m_Bullet->SetPosition({m_Bullet->GetPosition().x + BulletSpeed, m_Bullet->GetPosition().y});

    // **關閉窗口**
    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) || Util::Input::IfExit()) {
        m_CurrentState = State::END;
    }

    m_Root.Update();
}
