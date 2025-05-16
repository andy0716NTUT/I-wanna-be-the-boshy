void Game::Update() {
    // 獲取當前滑鼠位置（SDL 座標系統，原點在左上角）
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    
    // 獲取目標位置（遊戲座標系統）
    glm::vec2 targetPos = m_Boshy->GetTransform().translation;
    
    // 調試信息
    spdlog::info("Current SDL Pos: ({}, {})", mouseX, mouseY);
    spdlog::info("Target Pos: ({}, {})", targetPos.x, targetPos.y);
    
    // 檢查目標位置是否在合理範圍內
    if (targetPos.x < -1000 || targetPos.x > 1000 || targetPos.y < -1000 || targetPos.y > 1000) {
        spdlog::warn("Target position out of expected range!");
        return;
    }
    
    // 將遊戲座標轉換為 SDL 座標
    float targetSDLX = targetPos.x + 640;  // 轉換到 SDL 座標系統
    float targetSDLY = 360 - targetPos.y;  // 轉換到 SDL 座標系統
    
    spdlog::info("Target SDL Pos: ({}, {})", targetSDLX, targetSDLY);
    
    // 計算平滑跟隨（在 SDL 座標系統中）
    float newX = mouseX + (targetSDLX - mouseX) * 0.05f;
    float newY = mouseY + (targetSDLY - mouseY) * 0.05f;
    
    // 限制在視窗範圍內
    newX = std::max(0.0f, std::min(1280.0f, newX));
    newY = std::max(0.0f, std::min(720.0f, newY));
    
    // 調試信息
    spdlog::info("New SDL Pos: ({}, {})", newX, newY);
    
    // 直接使用 SDL 函數設置滑鼠位置
    SDL_WarpMouseInWindow(nullptr, static_cast<int>(newX), static_cast<int>(newY));
    
    // 更新所有遊戲物件
    for (auto &obj : m_GameObjects) {
        obj->Update();
    }
} 