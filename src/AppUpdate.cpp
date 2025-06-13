#include "App.hpp"

#include "spdlog/fmt/bundled/chrono.h"

void App::Update() {
    static float velocityY = 0;
    velocityY += Gravity;
    const float JumpPower = 16;
    const float MaxFallSpeed = -10;
    static int jumpCount = 0;
    static float shootCooldown = 0;
    const float deltaTime = 1.0f / 60.0f;    static float switchTimer = 0.0f; // 地圖切換
    static bool isSwitch = false; // 地圖切換狀態
    const float switchInterval = 1.0f; // 每1秒切換一次
    static bool keepExtending = true; // 平台延伸控制
    static int startTileX = 28; // 移到外部
    static int startTileY = 56; // 秮到外部
    static float platformTimer = 0.0f; // 移到外部
    shootCooldown -= deltaTime * 4;
    std::stringstream ss;
    bool needsRespawn = false;
    static bool isJumping = false;

    if (m_GamePhase == GamePhase::MENU) {
        m_Menu->Update(deltaTime);
        m_Root.Update();

        if (m_Menu->StartGame()) {
            m_GamePhase = GamePhase::WORLD1;
            m_Menu->SetVisible(false);
            m_Root.AddChild(m_PRM->GetChildren());
            m_Root.AddChild(m_Bullet);
            m_Root.AddChild(m_Boshy);

            currentX = 2 ;
            currentY = 5 ;

            CurrentWorld = GamePhaseToString(m_GamePhase);
            auto& currentWorld = m_World->GetWorldByPhaseName(CurrentWorld);
            std::tie(currentX, currentY) = m_World->GetStartPosition(currentWorld, "1");

            m_PRM->SetPhase(currentWorld[currentX][currentY],CurrentWorld);
            m_MapLoader->LoadMap(currentWorld[currentX][currentY],CurrentWorld);

            ReloadMapObjects();
            m_Boshy->SetPosition({-560,364});
            m_BGM->SetBGM("WORLD1");
        }
    }else{
        glm::vec2 position = m_Boshy->GetPosition();
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
        ss << "curentY: (" << currentY;
        m_DebugInfo.currentX = currentY;
        ss << "curentX: (" << currentX;
        m_DebugInfo.currentY = currentX;
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

        auto* animatedBoshy = dynamic_cast<AnimatedCharacter*>(m_Boshy.get());

        int belowTile = m_MapLoader->GetTile(tileX, tileY + 1);
        int aboveTile = m_MapLoader->GetTile(tileX, tileY - 1);
        int leftTile = m_MapLoader->GetTile(tileX - 1, tileY);
        int rightTile = m_MapLoader->GetTile(tileX + 1, tileY);
        int centerTile = m_MapLoader->GetTile(tileX, tileY);

        // 上帝模式下忽略致命碰撞
        if (!GodMode) {
            // 尖刺碰撞检测代码
            if (aboveTile == 5 || belowTile == 5 || leftTile == 5 || rightTile == 5 || centerTile == 5)
            {
                position = currentCheckPoint;
                currentX = checkPointX;
                currentY = checkPointY;
                deathType = DeathType::REAL_DEATH;
                // 不再使用needsRespawn和立即return，让死亡处理代码有机会执行
            }
        }

        if (needsRespawn) {
            // 更新界面
            m_Root.Update();
            return;
        }
        if ((aboveTile == 2) && velocityY > 0){
            position.y = 480 - ((tileY + 1) * 16);
            velocityY = 0;
        }

        if ((belowTile == 1 || belowTile == 8)  && velocityY < 0){
            position.y = 480 - ((tileY) * 16) - 4;
            velocityY = 0;
            jumpCount = 0;
        }
        if (m_MapLoader->GetTile(tileX, tileY) == 1 && velocityY >= 0){
            position.y = 480 - (tileY - 1) * 16;
            velocityY = 0;
        }
        if (!(belowTile == 1 || belowTile == 8)) {
            if (jumpCount == 0) jumpCount = 1;
        }
        for (auto& boost : m_jumpBoost) {
            boost->CheckInteraction(position, jumpCount);
            boost->UpdateState(deltaTime);
        }

        // 檢查角色是否處於死亡狀態
        bool isCharacterDead = (deathType == DeathType::REAL_DEATH);
        
        // 當角色死亡時，隱藏角色的繪製；當角色活著時，顯示角色
        m_Boshy->SetVisible(!isCharacterDead);

        // 只有在角色未死亡時處理移動輸入
        if (!isCharacterDead) {
            if (Util::Input::IsKeyPressed(Util::Keycode::RIGHT)){
                float prevX = position.x;
                if (rightTile != 2){
                    position.x += 5;
                    animatedBoshy->SetDirection(Character::direction::RIGHT);
                }
                if (position.x != prevX){
                    animatedBoshy->SetState(Character::MoveState::RUN);
                }
                else{
                    animatedBoshy->SetState(Character::MoveState::IDLE);
                }
            }
            if (Util::Input::IsKeyPressed(Util::Keycode::LEFT)){
                float prevX = position.x;
                if (leftTile != 2){
                    position.x -= 5;
                    animatedBoshy->SetDirection(Character::direction::LEFT);
                }
                if (position.x != prevX){
                    animatedBoshy->SetState(Character::MoveState::RUN_LEFT);
                }
                else{
                    animatedBoshy->SetState(Character::MoveState::IDLE_LEFT);
                }
            }
            if (Util::Input::IsKeyPressed(Util::Keycode::UP)){
                float prevY = position.y;
                if (aboveTile != 2){
                    // 確保上方不是障礙物
                    position.y += 5;
                }
                if (position.y != prevY){
                    animatedBoshy->SetState(Character::MoveState::RUN); // 可新增 RUN_UP 狀態
                }
                else{
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
                glm::vec2 bulletPos;
                glm::vec2 bulletDirVec = {1.0f, 0.0f}; // 預設向右
                float rotation = 0.0f;
                if (m_GamePhase == GamePhase::WORLD2 && CurrentPhase == "2") {
                    bulletPos = m_Boshy->m_Transform.translation;
                    rotation = m_PRM->GetBackground()->getRotation();
                    // 根據玩家面向決定基礎方向
                    float baseAngle = (m_Boshy->GetDirection() == Character::direction::LEFT) ? 3.1415926f : 0.0f;
                    float angle = baseAngle + rotation;
                    bulletDirVec = {std::cos(angle), -std::sin(angle)};
                } else {
                    bulletPos = m_Boshy->GetPosition();
                    bulletDirVec = (m_Boshy->GetDirection() == Character::direction::LEFT) ? glm::vec2(-1.0f, 0.0f) : glm::vec2(1.0f, 0.0f);
                }
                auto bullet = Bullet::CreateBullet(bulletPos, m_Boshy->GetDirection(), 2.0f, m_Root);
                bullet->SetDirectionVector(bulletDirVec); // 設定自訂方向
                m_Bullets.push_back(bullet);
                shootCooldown = 0.5f;
            }

            else if (Util::Input::IsKeyDown(Util::Keycode::Z)) {
                if (jumpCount < 2 || GodMode) {  // 上帝模式下無限跳躍
                    velocityY = JumpPower;
                    isJumping = true;
                    if (!GodMode) jumpCount++;  // 只有非上帝模式才計算跳躍次數

                    if (animatedBoshy->GetDirection() == Character::direction::LEFT)
                        animatedBoshy->SetState(Character::MoveState::JUMP_LEFT);
                    else
                        animatedBoshy->SetState(Character::MoveState::JUMP);
                }
            }
            else if (Util::Input::IsKeyUp(Util::Keycode::Z) && isJumping && velocityY > 0) {
                velocityY *= 0.4f;
                isJumping = false;
            }

            if (velocityY <= 0) {
                isJumping = false;
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
        }

        // 更新角色位置以跟隨背景旋轉
        if (m_PRM) {
            auto background = m_PRM->GetBackground();
            if (background) {
                animatedBoshy->UpdatePositionWithRotation(background->getRotation());
            }
        }

        // 子彈移動邏輯修正
        for (auto& bullet : m_Bullets) {
            bullet->UpdateWithCollision(deltaTime, m_MapLoader);
        }

        // 检查点碰撞检测
        for (auto& bullet : m_Bullets) {
            glm::vec2 checkpointPos;            if (bullet->CheckCheckpointCollision(m_CheckPoints, checkpointPos, currentCheckPointPhase, checkPointX, checkPointY)) {
                // 清空子彈
                bullet->SetVisible(false);
                bullet->SetDrawable(nullptr);
                // 不再直接設置檢查點位置，而是由延遲計時器處理
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

            ClearGameObjects(m_Bullets);
            m_Bullets.clear();
            ClearDrawables(m_CheckPoints);
            ClearDrawables(m_jumpBoost);

            std::string newPhase;
            if (CurrentPhase == "-") {
                newPhase = "none";
            } else if (m_GamePhase == GamePhase::WORLD1 && CurrentPhase == "3") {
                newPhase = "3_1";
            } else if (m_GamePhase == GamePhase::WORLD1 && CurrentPhase == "4") {
                newPhase = "4_1";
            }else if (m_GamePhase == GamePhase::WORLD2 && CurrentPhase == "2"){
                newPhase = "2_1";
            } else {
                newPhase = CurrentPhase;
            }
            if (m_phase8bird) {
                glm::vec2 birdPos = m_phase8bird->GetPosition();
                float remainingDistance = 640.0f - birdPos.x; // 以640為右邊界
                float delayTime = remainingDistance / m_phase8bird->GetSpeedX(); // 飛完剩下距離需要時間
                m_phase8bird->StartPending(delayTime, birdPos.y); // ✅ 啟動延遲
            }
            m_PRM->SetPhase(newPhase,CurrentWorld);
            m_MapLoader->LoadMap(newPhase,CurrentWorld);

            ReloadMapObjects();

            std::cout << "Current Phase : " << CurrentPhase << std::endl;
        }
        if ((CurrentPhase == "3" || CurrentPhase == "3_1" || CurrentPhase == "3_2") && m_GamePhase == GamePhase::WORLD1) {
            switchTimer += deltaTime;
            // 移除過度頻繁的輸出，避免降低性能
            if (switchTimer >= switchInterval)
            {
                isSwitch = !isSwitch;
                std::string newPhase = isSwitch ? "3_2" : "3_1";
                m_PRM->SetPhase(newPhase,CurrentWorld);
                m_MapLoader->LoadMap(newPhase,CurrentWorld);
                switchTimer = 0.0f; // 重置計時器
            }
        }
        if (CurrentPhase == "4" || CurrentPhase.find("4_") == 0) {
            switchTimer += deltaTime;
            if (switchTimer >= switchInterval) {
                isSwitch = !isSwitch;
                std::string newPhase = isSwitch ? "4_2" : "4_1";
                m_PRM->SetPhase(newPhase, CurrentWorld);
                m_MapLoader->LoadMap(newPhase, CurrentWorld);
                switchTimer = 0.0f;
            }
        }
        // === Bear 通用邏輯 (Phase4 & Phase5) ===
        if ((CurrentPhase.find("4") == 0 || CurrentPhase == "5")) {
            if (!m_bear) {
                m_bear = std::make_shared<bear>();
                m_bear->SetZIndex(-2);
                m_Root.AddChild(m_bear);
                std::cout << "Bear created for phase " << CurrentPhase << std::endl;
            }

            m_bear->SetMapInfoLoader(m_MapLoader);
            m_bear->SetCurrentPhase(CurrentPhase);

            std::string currentPhaseCopy = CurrentPhase;
            if (m_bear->detect(currentPhaseCopy)) {
                m_bear->SetVisible(true);
            }

            if (m_bear->exist()) {
                m_bear->Update(m_Boshy->GetPosition());
                // === Bear 與 Boshy 碰撞判定 ===
                glm::vec2 bearPos = m_bear->GetPosition();
                glm::vec2 boshyPos = m_Boshy->GetPosition();
                float collisionDistance = 40.0f; // 可依實際角色大小調整
                if (glm::distance(bearPos, boshyPos) < collisionDistance && !GodMode) {
                    deathType = DeathType::REAL_DEATH;
                }
            }
        } else if (m_bear) {
            m_Root.RemoveChild(m_bear);
            m_bear = nullptr;
            std::cout << "Bear completely released when leaving phase " << CurrentPhase << std::endl;
        }

        if ((m_GamePhase == GamePhase::WORLD1 && CurrentPhase == "2") && !trapCreated) {
            m_phase2trap_down = std::make_shared<phase2trap>();
            m_phase2trap_down->Create(
                m_MapLoader,
                RESOURCE_DIR"/Image/MapObject/phase2trap1",
                {0.0f, -480.0f},
                480.0f
            );
            m_Root.AddChild(m_phase2trap_down);

            m_phase2trap_up = std::make_shared<phase2trap>();
            m_phase2trap_up->Create(
                m_MapLoader,
                RESOURCE_DIR"/Image/MapObject/phase2trap2",
                {0.0f, 160.0f},
                190.0f
            );
            m_Root.AddChild(m_phase2trap_up);
            trapCreated = true;

        }else if ((m_GamePhase == GamePhase::WORLD1 && CurrentPhase != "2")&& trapCreated) {
            m_phase2trap_down->clear();
            m_phase2trap_up->clear();
            trapCreated = false;
        }
        if (CurrentPhase == "8") {
            static bool phase8Entered = false;
            static float phase8EnterTimer = 0.0f;
            glm::vec2 boshyPos = m_Boshy->GetPosition();

            // 進入 phase8 時重置計時器
            if (!phase8Entered) {
                phase8Entered = true;
                phase8EnterTimer = 0.0f;
            }
            phase8EnterTimer += deltaTime;

            // 只在延遲1秒後才開始偵測 x > 0.0f 並生成 spider
            if (!m_phase8spider && phase8EnterTimer > 1.0f && boshyPos.x > 0.0f) {
                m_phase8spider = std::make_shared<spider>();
                m_Root.AddChild(m_phase8spider);
                m_phase8spider->SetMapLoader(m_MapLoader);
                m_phase8spider->detect(boshyPos);
            }

            if (m_phase8spider) {
                m_phase8spider->Update(deltaTime);

                // spider 結束後再開始倒數出現 bird
                if (m_phase8spider->IsFinished() && !m_phase8bird) {
                    static float birdDelay = 0.0f;
                    birdDelay += deltaTime;
                    if (birdDelay > 1.0f) {
                        m_phase8bird = std::make_shared<Bird>();
                        m_phase8bird->Setposition({-640, -240});
                        m_Root.AddChild(m_phase8bird);
                        m_phase8bird->StartChase();
                        birdDelay = 0.0f; // 重置 timer
                    }
                }
            }
        } else {
            // 離開 phase8 時重置狀態
            static bool phase8Entered = false;
            static float phase8EnterTimer = 0.0f;
            phase8Entered = false;
            phase8EnterTimer = 0.0f;
        }

        // bird 持續追蹤邏輯（phase 8~12 都適用）
        if (m_phase8bird) {
            // 持續更新 bird 直到 phase12
            if (CurrentPhase == "8" || CurrentPhase == "9" || CurrentPhase == "10" || CurrentPhase == "11" || CurrentPhase == "12") {
                m_phase8bird->Update(deltaTime, m_Boshy->GetPosition());

                if (glm::distance(m_phase8bird->GetPosition(), m_Boshy->GetPosition()) < 20.0f && !GodMode) {
                    // 玩家被鸟击中，触发真实死亡
                    m_Boshy->SetPosition(currentCheckPoint);
                    currentX = checkPointX;
                    currentY = checkPointY;
                    deathType = DeathType::REAL_DEATH;
                    return;
                }
                if (CurrentPhase == "12" && m_phase8bird->GetPosition().x >= 0.0f) {
                    ClearGameObjects(m_phase8bird);
                    m_phase8bird = nullptr;
                }
            }
        }
        if (CurrentPhase == "13") {
            if (!m_Boss1) {
                m_Boss1 = std::make_shared<Boss1>();
                m_Root.AddChild(m_Boss1);
                m_Boss1->Spawn(deltaTime,m_Root);
                m_BGM->SetBGM("Boss1");
            }
            m_Boss1->Update(deltaTime,m_Boshy->GetPosition(),m_Root);
            // 只有在 Boss 死後才開始平台延伸
            if (m_Boss1->Boss1Finished()) {
                platformTimer += deltaTime;
                if (keepExtending && platformTimer >= 0.2f) {
                    if (startTileX < 80) {
                        m_MapLoader->SetTile(startTileX, startTileY, 8);
                        m_MapLoader->SetTile(startTileX + 1, startTileY, 8);
                        m_MapLoader->SetTile(startTileX, startTileY + 1, 8);
                        m_MapLoader->SetTile(startTileX + 1, startTileY + 1, 8);
                        auto newPlatforms = CreateGameObjectsFromMap<Platform>(m_MapLoader, m_Root);
                        m_Platform.insert(m_Platform.end(), newPlatforms.begin(), newPlatforms.end());
                        startTileX += 2;
                        platformTimer = 0.0f;
                        std::cout << "Platform extended to: " << startTileX << std::endl;
                    }
                }
            }
            for (auto& bullet : m_Bullets) {
                if (bullet && bullet->IsVisible()) {
                    glm::vec2 bulletPos = bullet->GetPosition();
                    glm::vec2 bossPos = m_Boss1->GetPosition();

                    if (glm::distance(bulletPos, bossPos) < 100.0f) {  // 可以調整碰撞距離
                        m_Boss1->TakeDamage(1);  // 自訂函式，等一下寫
                        bullet->SetVisible(false);
                        bullet->SetDrawable(nullptr);
                    }
                }
            }
            if (m_Boss1->playerDead() && !GodMode) {
                // 已删除设置玩家死亡动画的代码
                deathType = DeathType::REAL_DEATH;
            }
        } else {
            if (m_Boss1)ClearGameObjects(m_Boss1);
        }

        // 添加調試信息
        // 更新陷阱
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
        }        for (auto& checkpoint : m_CheckPoints)
        {
            // 設置檢查點目標為玩家位置 (用於子彈射擊方向計算)
            checkpoint->SetTargetPosition(m_Boshy->GetPosition());
            
            // 更新子彈發射延遲計時器
            checkpoint->UpdateBulletShootDelay(deltaTime);
            
            // 更新延遲儲存計時器
            glm::vec2 savePosition;
            if (checkpoint->UpdateSaveDelay(deltaTime, savePosition)) {
                // 延遲時間到，現在儲存檢查點
                currentCheckPoint = savePosition;
                currentCheckPointPhase = m_MapLoader->GetCurrentPhase();
                checkPointX = currentX;
                checkPointY = currentY;
                checkPointWorld = CurrentWorld;
                std::cout << "延遲保存檢查點位置: (" << savePosition.x << ", " << savePosition.y << ")" << std::endl;
            }
                // 檢查是否應該發射子彈 (在動畫的特定幀)
            if (checkpoint->ShouldShootBullet()) {
                // 創建朝向玩家的子彈（使用特殊的檢查點子彈）
                auto deathBullet = Bullet::CreateCheckpointBullet(
                    checkpoint->GetPosition(),
                    m_Boshy->GetPosition(),  // 直接传递玩家的实时位置
                    1.0f,  // 1秒後消失
                    m_Root
                );
                  // 設定直線移動，不需要玩家位置
                
                // 添加到子彈列表以進行管理
                m_CheckpointBullets.push_back(deathBullet);
                checkpoint->ResetShotStatus(); // 重置射擊狀態
            }
            
            // 處理玩家子彈與檢查點的碰撞
            glm::vec2 cpPos = checkpoint->GetPosition();
            for (auto& bullet : m_Bullets){
                if (bullet) {
                    glm::vec2 bulletPos = bullet->GetPosition();
                    if (glm::distance(cpPos, bulletPos) < 32.0f)
                    {
                        checkpoint->play();
                        checkpoint->SetTargetPosition(m_Boshy->GetPosition()); // 記錄當前玩家位置用於延遲儲存
                        ClearGameObjects(bullet);
                        // 不立即保存檢查點位置，等延遲計時器完成後再保存
                        break; // 跳出內層循環
                    }
                }
            }
        }
          // 處理檢查點子彈與玩家的碰撞
        for (auto& cpBullet : m_CheckpointBullets) {
            if (cpBullet && cpBullet->IsVisible()) {                // 更新子彈位置（使用直線移動，不會跟蹤玩家）
                cpBullet->UpdateCheckpointBullet(deltaTime);
                
                // 處理與地圖的碰撞
                int bulletTileX = static_cast<int>((cpBullet->GetPosition().x + 640) / 16);
                int bulletTileY = static_cast<int>((480 - cpBullet->GetPosition().y) / 16);
                
                if (bulletTileX >= 0 && bulletTileX < m_MapLoader->GetWidth() &&
                    bulletTileY >= 0 && bulletTileY < m_MapLoader->GetHeight()) {
                    
                    int tileValue = m_MapLoader->GetTile(bulletTileX, bulletTileY);
                    if (tileValue == 1 || tileValue == 2) {
                        cpBullet->SetVisible(false);
                        continue;  // 跳過後續處理
                    }
                }
                
                // 檢查與玩家的碰撞
                if (glm::distance(cpBullet->GetPosition(), m_Boshy->GetPosition()) < 20.0f && !GodMode) {
                    // 玩家被擊中，重置位置
                    position = currentCheckPoint;
                    currentX = checkPointX;
                    currentY = checkPointY;
                    needsRespawn = true;
                    deathType = DeathType::REAL_DEATH;
                    break;
                }
            }        }
        
        // 清理檢查點子彈 (從渲染器中移除)
        Bullet::CleanBullet(m_CheckpointBullets, m_Root);
        // 检查鼠标和角色是否重疊（使用 PTSD Position）
        // 清除不可見的子彈
        Bullet::CleanBullet(m_Bullets);
        // 關閉或重生邏輯
        if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) || Util::Input::IfExit())
        {
            m_CurrentState = State::END;
        }
        if (Util::Input::IsKeyDown(Util::Keycode::R)) {
            startTileX = 28;
            position = currentCheckPoint;
            Respawn();
        }
        // 更新角色位置與整體狀態
        m_Boshy->SetPosition(position);
        
        if (CurrentPhase == "14" && glm::distance(m_Boshy->GetPosition(),{448,-404}) < 20.0f) {
            m_GamePhase = GamePhase::WORLD2;
            CurrentWorld = GamePhaseToString(m_GamePhase);
            auto& currentWorld = m_World->GetWorldByPhaseName(CurrentWorld);
            std::tie(currentX, currentY) = m_World->GetStartPosition(currentWorld, "1");
            m_PRM->SetPhase(currentWorld[currentX][currentY],CurrentWorld);
            m_MapLoader->LoadMap(currentWorld[currentX][currentY],CurrentWorld);
            m_Boshy->SetPosition({-500,100});
            ReloadMapObjects();
            switchTimer = 0.0f; // 重置 switchTimer
            isSwitch = false;   // 重置切換狀態
            m_BGM->SetBGM("WORLD2"); // 進入WORLD2時只呼叫一次
        }
        //=================================================================WORLD2屎山===================================================================================
        // if (m_GamePhase == GamePhase::WORLD2)m_BGM->SetBGM("WORLD2"); // <-- 移除這行

        if (m_GamePhase == GamePhase::WORLD2 && CurrentPhase == "2") {
            // 只旋轉背景
            m_PRM->rotate(deltaTime);
            // 更新角色的視覺旋轉，但不改變實際位置
            m_Boshy->UpdatePositionWithRotation(m_PRM->GetBackground()->getRotation());
            switchTimer += deltaTime;
            if (switchTimer >= switchInterval)
            {
                isSwitch = !isSwitch;
                std::string newPhase = isSwitch ? "2_2" : "2_1";
                m_PRM->SetPhase(newPhase,CurrentWorld);
                m_MapLoader->LoadMap(newPhase,CurrentWorld);
                switchTimer = 0.0f; // 重置計時器
            }        }else if (m_GamePhase == GamePhase::WORLD2) {
            // 只在World2的其他階段重置旋轉，但不影響World1的計時器
            m_PRM->resetRotation();
            m_Boshy->UpdatePositionWithRotation(0.0f); // 重置角色的旋轉
        }
        if (deathType != DeathType::NONE) {
            // 处理不同类型的死亡
            if (deathType == DeathType::REAL_DEATH) {
                // 真正死亡状态：显示死亡UI和动画
                m_GameOverUI->Show();
                
                // 更新死亡動畫
                deathAnimTimer += deltaTime;
                
                // 死亡動畫播放0.8秒後才能按R鍵重置
                if (deathAnimTimer >= 0.8f) {
                    deathAnimFinished = true;
                    
                    // 如果按R鍵，則重置
                    if (Util::Input::IsKeyPressed(Util::Keycode::R)) {
                        m_GameOverUI->Hide(); // 立即隱藏死亡UI
                        deathType = DeathType::NONE;
                        Respawn();
                    } else {
                        m_GameOverUI->Update(deltaTime);
                    }
                } else {
                    m_GameOverUI->Update(deltaTime);
                }
            } else if (deathType == DeathType::RESET) {
                // 直接重置，不顯示死亡動畫
                m_GameOverUI->Hide(); // 确保死亡UI被隐藏
                deathType = DeathType::NONE;
                Respawn();
            }
        }
        m_Root.Update();
        RenderImGui(*this);
    }
}