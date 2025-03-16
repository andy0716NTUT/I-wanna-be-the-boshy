#include "Menu.hpp"
#include "Util/Logger.hpp"  // 用於調試輸出（可選）

// 構造函數：初始化選單動畫和選項
Menu::Menu() : m_CurrentIndex(0), m_SelectedOption(Option::START_GAME) {
    // 加載選單動畫的圖片序列（假設有 40 張圖片）
    std::vector<std::string> menuImages;
    for (int i = 1; i <= 40; ++i) {
        menuImages.push_back(RESOURCE_DIR"/Image/Menu/Menu" + std::to_string(i) + ".png");
    }

    // 創建動畫對象
    // 參數：圖片列表、是否循環播放、Z 軸層級、是否自動播放、幀間隔（ms）
    m_Animation = std::make_shared<Util::Animation>(menuImages, true, -10, true, 100);
    m_Animation->Play();  // 啟動動畫

    // 設置動畫為可渲染內容
    SetDrawable(m_Animation);

    // 設置 Z 軸層級（與動畫一致）
    SetZIndex(-10);
}

// 更新方法：處理輸入和動畫邏輯
void Menu::Update() {
    HandleInput();  // 處理用戶輸入
    // 注意：如果 Util::Animation 會自動更新，這裡不需要額外調用 m_Animation->Update()
}

// 輸入處理方法：檢測按鍵並切換或確認選項
void Menu::HandleInput() {
    if (Util::Input::IsKeyDown(Util::Keycode::DOWN)) {
        // 向下切換選項，模 3 表示有 3 個選項
        m_CurrentIndex = (m_CurrentIndex + 1) % 3;
        LOG_DEBUG("Menu index: " + std::to_string(m_CurrentIndex));  // 調試輸出
    } else if (Util::Input::IsKeyDown(Util::Keycode::UP)) {
        // 向上切換選項，模 3 確保循環
        m_CurrentIndex = (m_CurrentIndex + 2) % 3;  // +2 是模 3 的逆向計算
        LOG_DEBUG("Menu index: " + std::to_string(m_CurrentIndex));
    } else if (Util::Input::IsKeyDown(Util::Keycode::RETURN)) {
        // 按 Enter 鍵確認當前選項
        switch (m_CurrentIndex) {
            case 0:
                m_SelectedOption = Option::START_GAME;
            LOG_DEBUG("Selected: START_GAME");
            break;
            case 1:
                m_SelectedOption = Option::SETTINGS;
            LOG_DEBUG("Selected: SETTINGS");
            break;
            case 2:
                m_SelectedOption = Option::EXIT;
            LOG_DEBUG("Selected: EXIT");
            break;
        }
    }
}