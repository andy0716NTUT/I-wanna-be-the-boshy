//
// Created by andyl on 2025/3/8.
//
#include "App.hpp"


void App::Update() {
    static float velocityY = 0;       // 垂直速度
    const float Gravity = -1;         // 重力
    const float JumpPower = 15;       // 跳躍力量
    const float MaxFallSpeed = -20;   // 最大下落速度
    static int jumpCount = 0;         // 記錄已經跳了幾次（最多 2）
    static float shootCooldown = 0;   // 射擊冷卻時間

    const float deltaTime = 1.0f / 60.0f; // 假設 60 FPS
    shootCooldown -= deltaTime; // 減少冷卻時間

    glm::vec2 position = m_Boshy->GetPosition();

    // 施加重力
    velocityY += Gravity;
    if (velocityY < MaxFallSpeed) velocityY = MaxFallSpeed;
    position.y += velocityY;

    // 地板碰撞檢測
    if (position.y <= -128) {
        position.y = -128;
        velocityY = 0;
        jumpCount = 0;
    }

    // 跳躍邏輯
    if (Util::Input::IsKeyDown(Util::Keycode::Z) && jumpCount < 2) {
        velocityY = JumpPower;
        jumpCount++;
    }

    // 左右移動
    if (Util::Input::IsKeyPressed(Util::Keycode::RIGHT)) {
        m_Boshy->SetImage(RESOURCE_DIR"/Image/Character/idle/idle1-right.png");
        position.x += 5;
    }
    if (Util::Input::IsKeyPressed(Util::Keycode::LEFT)) {
        m_Boshy->SetImage(RESOURCE_DIR"/Image/Character/idle/idle1-left.png");
        position.x -= 5;
    }

    // 更新角色位置
    m_Boshy->SetPosition(position);

    // 關閉窗口邏輯
    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) || Util::Input::IfExit()) {
        m_CurrentState = State::END;
    }

    m_Root.Update(); // 渲染樹自動處理所有可見對象的繪製
}