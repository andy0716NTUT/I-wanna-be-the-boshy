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
    static int startTileY = 56; // 移到外部
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
            m_Boshy->SetPosition({525,-372});
            m_BGM->SetBGM(RESOURCE_DIR"/BGM/WORLD1.mp3");
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
            // 原有的碰撞檢測代碼
            if (aboveTile == 5 || belowTile == 5 || leftTile == 5 || rightTile == 5 || centerTile == 5)
            {
                position = currentCheckPoint;
                currentX = checkPointX;
                currentY = checkPointY;
                needsRespawn = true;
                Respawn();
            }
        }

        if (needsRespawn) {
            // 更新界面
            m_Root.Update();
            return;
        }
        if ((aboveTile == 2 || aboveTile == 1) && velocityY > 0){
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
            auto bullet = Bullet::CreateBullet(m_Boshy->GetPosition(), m_Boshy->GetDirection(), 2.0f, m_Root);
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
            glm::vec2 checkpointPos;
            if (bullet->CheckCheckpointCollision(m_CheckPoints, checkpointPos, currentCheckPointPhase, checkPointX, checkPointY)) {
                // 清空所有子弹
                bullet->SetVisible(false);
                bullet->SetDrawable(nullptr);
                currentCheckPoint = m_Boshy->GetPosition();
                checkPointWorld = CurrentWorld;
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
        }        if (CurrentPhase == "3" || CurrentPhase == "3_1" || CurrentPhase == "3_2")
        {
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
        }if (CurrentPhase == "4" || CurrentPhase.find("4_") == 0) {
            switchTimer += deltaTime;
            if (switchTimer >= switchInterval) {
                isSwitch = !isSwitch;
                std::string newPhase = isSwitch ? "4_2" : "4_1";

                m_PRM->SetPhase(newPhase,CurrentWorld);
                m_MapLoader->LoadMap(newPhase,CurrentWorld);
                switchTimer = 0.0f;
            }
        
            // Detect the bear if it exists
            if (m_bear) {
                // Set the MapInfoLoader and current phase for the bear
                m_bear->SetMapInfoLoader(m_MapLoader);
                m_bear->SetCurrentPhase(CurrentPhase);
                
                std::string currentPhaseCopy = CurrentPhase; // Create a copy for detect
            
                if (m_bear->detect(currentPhaseCopy)) {
                    m_bear->SetVisible(true);
                    std::cout << "Bear detected in phase " + currentPhaseCopy << std::endl;
                    LOG_TRACE("Bear detected in phase " + currentPhaseCopy);
                }
                
                // Update the bear if it is active
                if (m_bear->exist()) {
                    m_bear->Update(m_Boshy->GetPosition());
                }
            }
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
            glm::vec2 boshyPos = m_Boshy->GetPosition();

            if (!m_phase8spider && boshyPos.x > -0.0f) {
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
        }

        // bird 持續追蹤邏輯（phase 8~12 都適用）
        if (m_phase8bird) {
            // 持續更新 bird 直到 phase12
            if (CurrentPhase == "8" || CurrentPhase == "9" || CurrentPhase == "10" || CurrentPhase == "11" || CurrentPhase == "12") {
                m_phase8bird->Update(deltaTime, m_Boshy->GetPosition());

                if (glm::distance(m_phase8bird->GetPosition(), m_Boshy->GetPosition()) < 20.0f && !GodMode) {
                    m_Boshy->SetPosition(currentCheckPoint);
                    currentX = checkPointX;
                    currentY = checkPointY;
                    Respawn();
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
                m_BGM->SetBGM(RESOURCE_DIR"/BGM/BOSS1.mp3");
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
                Respawn();
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
        }
        for (auto& checkpoint : m_CheckPoints)
        {
            glm::vec2 cpPos = checkpoint->GetPosition();
            for (auto& bullet : m_Bullets)
            {
                if (bullet) {
                    glm::vec2 bulletPos = bullet->GetPosition();
                    if (glm::distance(cpPos, bulletPos) < 32.0f)
                    {
                        checkpoint->play();
                        ClearGameObjects(bullet);
                        currentCheckPoint = m_Boshy->GetPosition();
                        currentCheckPointPhase = m_MapLoader->GetCurrentPhase();
                        checkPointX = currentX;
                        checkPointY = currentY;
                        break; // 跳出內層循環
                    }
                }

            }
        }
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
        
        // 計算目標位置（boshy的位置加上偏移量）
        glm::vec2 targetPos = m_Boshy->GetPosition() + glm::vec2(0, 20); // 向上偏移20單位
        // 獲取當前鼠標位置
        Util::PTSDPosition currentPtsdPos = Util::Input::GetCursorPosition();
        // 將PTSD座標轉換為遊戲座標
        glm::vec2 currentPos(currentPtsdPos.x - 640, 480 - currentPtsdPos.y);
        
        // 檢查鼠標和角色是否重疊（使用 PTSD Position）
        glm::vec2 boshyPtsdPos = {m_Boshy->GetPosition().x + 640, -(480 - m_Boshy->GetPosition().y)};
        float distance = glm::distance(glm::vec2(currentPtsdPos.x, -currentPtsdPos.y), boshyPtsdPos);
        bool isOverlapping = (distance >= 790.0f && distance <= 810.0f) && !GodMode;
        // if (isOverlapping) {
        //     std::cout << "Mouse collision! Distance: " << distance << std::endl;
        //     position = currentCheckPoint; // 傳回到檢查點
        //     currentX = checkPointX;
        //     currentY = checkPointY;
        //     needsRespawn = true;
        //     Respawn();
        // }
        
        // 更新 ImGui 調試信息
        ss.str("");
        ss << "Mouse PTSD Position: (" << currentPtsdPos.x << ", " << currentPtsdPos.y << ")";
        m_DebugInfo.mousePtsdPos = ss.str();
        ss.str("");
        ss << "Mouse Game Position: (" << currentPos.x << ", " << currentPos.y << ")";
        m_DebugInfo.mouseGamePos = ss.str();
        ss.str("");
        ss << "Boshy PTSD Position: (" << boshyPtsdPos.x << ", " << boshyPtsdPos.y << ")";
        m_DebugInfo.boshyPtsdPos = ss.str();
        ss.str("");
        ss << "Distance: " << distance << " | Overlapping: " << (isOverlapping ? "Yes" : "No") << " | Range: 750-850";
        m_DebugInfo.overlapInfo = ss.str();
        
        // 計算平滑跟隨
        glm::vec2 newPos = currentPos + (targetPos - currentPos) * 0.05f; // 降低平滑係數使移動更平滑
        // 將遊戲座標轉換回PTSD座標
        
        // 更新角色位置與整體狀態
        m_Boshy->SetPosition(position);
        
        if (CurrentPhase == "14" && glm::distance(m_Boshy->GetPosition(),{300,-404}) < 20.0f) {
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
        }
        //=================================================================WORLD2屎山===================================================================================
        if (m_GamePhase == GamePhase::WORLD2)m_BGM->SetBGM(RESOURCE_DIR"/BGM/WORLD2.mp3");
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
        m_Root.Update();
        RenderImGui(*this);
    }
}