#include "App.hpp"

#include "spdlog/fmt/bundled/chrono.h"

void App::Update() {
    static float velocityY = 0;
    velocityY += Gravity;
    const float JumpPower = 15;
    const float MaxFallSpeed = -10;
    static int jumpCount = 0;
    static float shootCooldown = 0;
    const float deltaTime = 1.0f / 60.0f;
    static float switchTimer = 0.0f; // 地圖切換
    static bool isSwitch = false; // 地圖切換狀態
    const float switchInterval = 1.0f; // 每3秒切換一次
    shootCooldown -= deltaTime * 4;
    std::stringstream ss;
    bool needsRespawn = false;

    glm::vec2 position = m_Boshy->GetPosition();
    auto* animatedBoshy = dynamic_cast<AnimatedCharacter*>(m_Boshy.get());
    // TileX, TileY 計算與碰撞檢測部分保持不變
    int tileX = static_cast<int>((position.x + 640) / 16);
    int tileY = static_cast<int>((480 - position.y) / 16);

    if (tileX < 0) tileX = 0;
    if (tileX >= m_MapLoader->GetWidth()) tileX = m_MapLoader->GetWidth() - 1;
    if (tileY < 0) tileY = 0;
    if (tileY >= m_MapLoader->GetHeight()) tileY = m_MapLoader->GetHeight() - 1;

    //imgui settings
    ss << "Position: (" << position.x << ", " << position.y << ")";
    m_DebugInfo.positionInfo = ss.str();
    ss.str("");
    ss << "Tile: (" << tileX << ", " << tileY << ") Value: " << m_MapLoader->GetTile(tileX, tileY);
    m_DebugInfo.tileInfo = ss.str();
    ss.str("");
    ss << "Switch Timer: " << switchTimer;
    m_DebugInfo.switchTimerInfo = ss.str();
    ss.str("");
    ss << "Current Phase: " << CurrentPhase;
    m_DebugInfo.phaseInfo = ss.str();

    velocityY += Gravity; // Gravity

    if (velocityY < MaxFallSpeed) velocityY = MaxFallSpeed;
    position.y += velocityY;

    int belowTile = m_MapLoader->GetTile(tileX, tileY + 1);
    int aboveTile = m_MapLoader->GetTile(tileX, tileY - 1);
    int leftTile = m_MapLoader->GetTile(tileX - 1, tileY);
    int rightTile = m_MapLoader->GetTile(tileX + 1, tileY);

    if (aboveTile == 5 || belowTile == 5 || leftTile == 5 || rightTile == 5)
    {
        position = currentCheckPoint; // 傳回到檢查點
        currentX = checkPointX;
        currentY = checkPointY;
        needsRespawn = true;
        Respawn(); // 呼叫重生邏輯
    }
    if (needsRespawn) {
        // 更新界面
        m_Root.Update();
        return;
    }
    if ((aboveTile == 2 || aboveTile == 1) && velocityY > 0)
    {
        position.y = 480 - ((tileY + 1) * 16);
        velocityY = 0;
    }

    if ((belowTile == 1 || belowTile == 8)  && velocityY < 0)
    {
        position.y = 480 - ((tileY) * 16) - 4;
        velocityY = 0;
        jumpCount = 0;
    }

    if (m_MapLoader->GetTile(tileX, tileY) == 1 && velocityY >= 0)
    {
        position.y = 480 - (tileY - 1) * 16;
        velocityY = 0;
    }
    for (auto& boost : m_jumpBoost) {
        boost->CheckInteraction(position, jumpCount);
        boost->UpdateState(deltaTime);
    }

    if (Util::Input::IsKeyPressed(Util::Keycode::RIGHT))
    {
        float prevX = position.x;
        if (rightTile != 2)
        {
            position.x += 5;
            animatedBoshy->SetDirection(Character::direction::RIGHT);
        }
        if (position.x != prevX)
        {
            animatedBoshy->SetState(Character::MoveState::RUN);
        }
        else
        {
            animatedBoshy->SetState(Character::MoveState::IDLE);
        }
    }
    if (Util::Input::IsKeyPressed(Util::Keycode::LEFT))
    {
        float prevX = position.x;
        if (leftTile != 2)
        {
            position.x -= 5;
            animatedBoshy->SetDirection(Character::direction::LEFT);
        }
        if (position.x != prevX)
        {
            animatedBoshy->SetState(Character::MoveState::RUN_LEFT);
        }
        else
        {
            animatedBoshy->SetState(Character::MoveState::IDLE_LEFT);
        }
    }
    if (Util::Input::IsKeyPressed(Util::Keycode::UP))
    {
        float prevY = position.y;
        if (aboveTile != 2)
        {
            // 確保上方不是障礙物
            position.y += 5;
        }
        if (position.y != prevY)
        {
            animatedBoshy->SetState(Character::MoveState::RUN); // 可新增 RUN_UP 狀態
        }
        else
        {
            animatedBoshy->SetState(Character::MoveState::IDLE);
        }
    }

    if (Util::Input::IsKeyPressed(Util::Keycode::DOWN)){
        float prevY = position.y;
        if (belowTile != 2){
            // 確保下方不是障礙物
            position.y -= 5;
        }
        if (position.y != prevY){
            animatedBoshy->SetState(Character::MoveState::RUN); // 可新增 RUN_DOWN 狀態
        }
        else{
            animatedBoshy->SetState(Character::MoveState::IDLE);
        }
    }
    shootCooldown -= deltaTime;
    // 射擊邏輯保持不變
    if (shootCooldown <= 0 && Util::Input::IsKeyPressed(Util::Keycode::X) && m_Bullets.size() < 5) {
        auto bullet = Bullet::CreateBullet(m_Boshy->GetPosition(), m_Boshy->GetDirection(), 2.0f, m_Root);
        m_Bullets.push_back(bullet);
        shootCooldown = 0.5f;
    }

    else if (Util::Input::IsKeyDown(Util::Keycode::Z) && jumpCount < 2)
    {
        velocityY = JumpPower;
        jumpCount++;
        if (animatedBoshy->GetDirection() == Character::direction::LEFT)
        {
            animatedBoshy->SetState(Character::MoveState::JUMP_LEFT);
        }
        else
        {
            animatedBoshy->SetState(Character::MoveState::JUMP);
        }
    }
    else if (animatedBoshy->IfAnimationEnds() || (animatedBoshy->GetState() != Character::MoveState::JUMP && velocityY
        == 0))
    {
        if (animatedBoshy->GetDirection() == Character::direction::LEFT)
        {
            animatedBoshy->SetState(Character::MoveState::IDLE_LEFT);
        }
        else
        {
            animatedBoshy->SetState(Character::MoveState::IDLE);
        }
    }

    // 子彈移動邏輯修正
    for (auto& bullet : m_Bullets) {
        bullet->UpdateWithCollision(deltaTime, m_MapLoader);
    }

    // 检查点碰撞检测
    for (auto& bullet : m_Bullets) {
        glm::vec2 checkpointPos;
        if (bullet->CheckCheckpointCollision(m_CheckPoints, checkpointPos, currentCheckPointPhase, checkPointX, checkPointY)) {
            // 清空所有子弹
            bullet->SetVisible(false);
            bullet->SetDrawable(nullptr);
            currentCheckPoint = m_Boshy->GetPosition();
            break;
        }
    }

    int tile = m_MapLoader->GetTile(tileX, tileY);
    World::Direction dir = m_World->GetTeleportDirection(tile);

    if (dir != World::Direction::NONE)
    {
        if (dir == World::Direction::RIGHT) {
            ++currentY;
            position.x *= -1;
            position.x += 16;
        }
        if (dir == World::Direction::LEFT) {
            --currentY;
            position.x *= -1;
            position.x -= 16;
        }
        if (dir == World::Direction::UP) {
            --currentX;
            position.y *= -1;
            position.y += 32;
        }
        if (dir == World::Direction::DOWN) {
            ++currentX;
            position.y *= -1;
            position.y -= 32;
        }

        CurrentPhase = m_World->GetWorldByPhaseName(GamePhaseToString(m_GamePhase))[currentX][currentY];

        for (auto& bullet : m_Bullets) {
            bullet->SetVisible(false);
            bullet->SetDrawable(nullptr);
        }
        m_Bullets.clear();
        for (auto& checkpoint : m_CheckPoints) {
            checkpoint->SetDrawable(nullptr);
        }
        for (auto& jumpboost : m_jumpBoost) {
            jumpboost->SetDrawable(nullptr);
        }
        std::string newPhase;
        if (CurrentPhase == "-") {
            newPhase = "none";
        } else if (CurrentPhase == "3") {
            newPhase = "3_1";
        } else if (CurrentPhase == "4") {
            newPhase = "4_1";
        } else {
            newPhase = CurrentPhase;
        }

        m_PRM->SetPhase(newPhase);
        m_MapLoader->LoadMap(newPhase);

        ClearGameObjects(m_Platform);
        ClearGameObjects(m_FallingGround);
        m_CheckPoints.clear();
        m_jumpBoost.clear();
        m_CheckPoints = CheckPoint::CreateFromMap(m_MapLoader, m_Root);
        m_jumpBoost = JumpBoost::CreateFromMap(m_MapLoader, m_Root);
        m_FallingGround = FallingGround::CreateFromMap(m_MapLoader,m_Root);
        m_Platform = Platform::CreateFromMap(m_MapLoader,m_Root);

        std::cout << "Current Phase : " << CurrentPhase << std::endl;
    }
    if (CurrentPhase == "3" || CurrentPhase == "3_1" || CurrentPhase == "3_2")
    {
        switchTimer += deltaTime;
        std::cout << switchTimer << std::endl;
        if (switchTimer >= switchInterval)
        {
            isSwitch = !isSwitch;
            std::string newPhase = isSwitch ? "3_2" : "3_1";
            m_PRM->SetPhase(newPhase);
            m_MapLoader->LoadMap(newPhase);
            switchTimer = 0.0f; // 重置計時器
        }
    }
    if (CurrentPhase == "4" || CurrentPhase.find("4_") == 0) {
        switchTimer += deltaTime;
        if (switchTimer >= switchInterval) {
            isSwitch = !isSwitch;
            std::string newPhase = isSwitch ? "4_2" : "4_1";

            // 只在4系列地图间切换时保留熊敌人
            glm::vec2 bearPosition;
            Character::direction bearDirection = Character::direction::RIGHT;
            std::shared_ptr<Enemy> bearEnemy = nullptr;

            // 寻找熊敌人
            for (auto& enemy : m_Enemies) {
                if (enemy->GetType() == Enemy::EnemyType::BASIC) {
                    bearPosition = enemy->GetPosition();
                    bearDirection = enemy->GetDirection();
                    bearEnemy = enemy;
                    break;
                }
            }

            // 更新资源和地图
            m_PRM->SetPhase(newPhase);
            m_MapLoader->LoadMap(newPhase);
            switchTimer = 0.0f;

            // 保留熊敌人，只清除其他敌人
            if (bearEnemy && (newPhase.find("4") == 0)) {
                for (auto& enemy : m_Enemies) {
                    if (enemy != bearEnemy) {
                        enemy->SetVisible(false);
                        enemy->SetDrawable(nullptr);
                    }
                }

                m_Enemies.clear();
                m_Enemies.push_back(bearEnemy);
            } else {
                // 正常清理并重建敌人
                ClearGameObjects(m_Enemies);
                m_Enemies.clear();
                m_Enemies = Enemy::CreateFromMap(m_MapLoader, m_Root);
            }
        }
    }
    if (CurrentPhase == "2" && !trapCreated) {
        m_phase2trap_down = std::make_shared<phase2trap>();
        m_phase2trap_down->Create(
            RESOURCE_DIR"/Image/MapObject/phase2trap1.png",
            {0.0f, -480.0f},
            480.0f
        );
        m_Root.AddChild(m_phase2trap_down);

        m_phase2trap_up = std::make_shared<phase2trap>();
        m_phase2trap_up->Create(
            RESOURCE_DIR"/Image/MapObject/phase2trap2.png",
            {0.0f, 160.0f},
            190.0f
        );
        m_Root.AddChild(m_phase2trap_up);

        trapCreated = true;
    }else if (CurrentPhase != "2" && trapCreated) {
        m_phase2trap_down->clear();
        m_phase2trap_up->clear();
        trapCreated = false;
    }


    // Update陷阱
    if (trapCreated) {
        if (m_phase2trap_down) m_phase2trap_down->Update(deltaTime);
        if (m_phase2trap_up) m_phase2trap_up->Update(deltaTime);
    }
    for (auto& fallingGround : m_FallingGround) {
        glm::vec2 fgPos = fallingGround->GetPosition();
        const float fgWidth = 96;
        const float fgHeight = 64;

        if (IsOnTop(position, fgPos, fgWidth, fgHeight)) {
            fallingGround->SetFalling(true);
        }
        if (fallingGround->GetFalling()) {
            fgPos.y -= 11;
            fallingGround->SetPosition(fgPos);
            for (auto& platform : m_Platform) {
                glm::vec2 pfPos = platform->GetPosition();
                const float pfWidth = 32;
                const float pfHeight = 32;

                if (IsAABBOverlap(fgPos, fgWidth, fgHeight, pfPos, pfWidth, pfHeight, 10.0f)) {
                    platform->SetVisible(false);
                    platform->SetDrawable(nullptr);

                    int tileX = static_cast<int>((pfPos.x + 640 - 16) / 16);
                    int tileY = static_cast<int>((480 - pfPos.y - 16) / 16);

                    for (int dy = 0; dy < 2; ++dy) {
                        for (int dx = 0; dx < 2; ++dx) {
                            m_MapLoader->SetTile(tileX + dx, tileY + dy, 0);
                        }
                    }

                    if (position.y > pfPos.y) {
                        position.y -= 2.0f;
                    }
                }
            }
        }
    }
        for (auto& checkpoint : m_CheckPoints)
        {
            glm::vec2 cpPos = checkpoint->GetPosition();
            for (auto& bullet : m_Bullets)
            {
                glm::vec2 bulletPos = bullet->GetPosition();
                if (glm::distance(cpPos, bulletPos) < 20.0f)
                {
                    checkpoint->play();
                    for (auto& bullet : m_Bullets)
                    {
                        bullet->SetVisible(false);
                        bullet->SetDrawable(nullptr); // 清除圖片資源
                    }
                    m_Bullets.clear();
                    currentCheckPoint = m_Boshy->GetPosition();
                    currentCheckPointPhase = m_MapLoader->GetCurrentPhase();
                    checkPointX = currentX;
                    checkPointY = currentY;
                    break; // 跳出內層循環
                }
            }
        }
    for (auto& enemy : m_Enemies) {
        enemy->Update(deltaTime, m_MapLoader, m_Boshy->GetPosition());

        // 检查子弹碰撞
        if (enemy->CheckBulletCollision(m_Bullets)) {
            enemy->TakeDamage(1);
        }

        // 检查玩家碰撞
        // 检查玩家碰撞
        if (enemy->CheckPlayerCollision(position)) {
            position = currentCheckPoint; // 传回到检查点
            currentX = checkPointX;
            currentY = checkPointY;
            needsRespawn = true;
            Respawn(); // 调用重生逻辑
            break;
        }
    }
        // 清除不可見的子彈
        Bullet::CleanBullet(m_Bullets);
        // 關閉或重生邏輯
        if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) || Util::Input::IfExit())
        {
            m_CurrentState = State::END;
        }
        if (Util::Input::IsKeyDown(Util::Keycode::R)) {
            position = currentCheckPoint;
            currentX = checkPointX;
            currentY = checkPointY;
            Respawn();
        }
        // 更新角色位置與整體狀態
        m_Boshy->SetPosition(position);
        m_Root.Update();
        RenderImGui(*this);
    }