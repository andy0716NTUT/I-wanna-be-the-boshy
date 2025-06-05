#include "bear.hpp"


bear::bear() {
    std::vector<std::string> frame1;
    std::vector<std::string> frame2;
    frame1.push_back(RESOURCE_DIR"/Image/enemy/bear1.png");
    frame1.push_back(RESOURCE_DIR"/Image/enemy/bear2.png");
    frame2.push_back(RESOURCE_DIR"/Image/enemy/bear3.png");
    frame2.push_back(RESOURCE_DIR"/Image/enemy/bear4.png");
    m_Animation_left = std::make_shared<Util::Animation>(frame1, true, 300, true);
    m_Animation_right = std::make_shared<Util::Animation>(frame2, true, 300, true);
    
    // Set the initial drawable based on default direction (left)
    m_Animation_left->Play();
    m_Drawable = m_Animation_left;
    
    // Set z-index for proper rendering (negative value to be behind player)
    SetZIndex(-2);
}
bool bear::detect(std::string &phase) {
    // Check if player has left phases where the bear should be active
    if ((phase != "4" && phase.find("4_") == std::string::npos && phase != "5") && this->spawn) {
        // Clear the bear when player leaves relevant phases
        this->spawn = false;
        SetVisible(false);
        
        // Pause the animations to free resources when the bear is cleared
        if (m_Animation_left) {
            m_Animation_left->Pause();
        }
        if (m_Animation_right) {
            m_Animation_right->Pause();
        }
        
        return false;
    }
    
    // Original spawning logic
    if ((phase == "4" || phase.find("4_") != std::string::npos) && !this->spawn) {
        this->spawn = true;
        this->attack = AttackType::CHASE;
        // Set the bear's respawn position - moved up to prevent sinking into floor
        this->SetPosition(glm::vec2(275, -120));
        
        // Make sure the animation is set and playing
        if (dir == direction::left) {
            m_Drawable = m_Animation_left;
            m_Animation_left->Play();
        } else {
            m_Drawable = m_Animation_right;
            m_Animation_right->Play();
        }
        
        // Set visibility to true to ensure it's drawn
        SetVisible(true);
        
        return true;
    }else if (phase == "5" && !this->spawn) {
        this->SetPosition(glm::vec2(420, 350));  // 初始位置 - moved up
        this->spawn = true;
        this->attack = AttackType::ROUND;
        SetVisible(true);

        // 動畫設定（必要）
        if (dir == direction::left) {
            m_Drawable = m_Animation_left;
            m_Animation_left->Play();
        } else {
            m_Drawable = m_Animation_right;
            m_Animation_right->Play();
        }

        return true;
    }
    return false;
}
bool bear::exist() {
    return this->spawn;
}
glm::vec2 bear::GetPosition() {
    return this->m_Transform.translation;
}
void bear::SetPosition(glm::vec2 position) {
    m_Transform.translation = position;
}

void bear::Update(glm::vec2 playerPosition) {
    if (dir == direction::left) m_Drawable = m_Animation_left;
    else m_Drawable = m_Animation_right;

    const float deltaTime = 1.0f / 60.0f;
    float& bearX = m_Transform.translation.x;
    float& bearY = m_Transform.translation.y;

    // === Phase4 ===
    if (m_CurrentPhase.find("4") == 0) {
        // -------- Phase4-Down --------
        if (playerPosition.y < 300) {
            const glm::vec2 range(-529.0f, 290.0f);
            static float velocity = 0.0f;
            static float turnCooldown = 0.0f;
            const float accel = 0.2f;
            const float maxSpeed = 8.0f;

            bearY = -120.0f;  // Moved up from -148.0f

            if (turnCooldown > 0.0f) {
                turnCooldown -= deltaTime;
                bearX += (dir == direction::left ? -velocity : velocity) * 0.5f; // 減速滑行
                return;
            }

            float dx = playerPosition.x - bearX;
            direction desiredDir = (dx < 0) ? direction::left : direction::right;

            if (dir != desiredDir) {
                velocity *= 0.3f;            // 模擬急煞
                turnCooldown = 0.15f;        // 滑一下再轉身
                dir = desiredDir;
            }

            velocity = std::min(velocity + accel, maxSpeed);
            bearX += (dir == direction::left) ? -velocity : velocity;

            // 邊界限制與強制轉向
            if (bearX < range.x) {
                bearX = range.x; dir = direction::right; velocity = 0.0f; turnCooldown = 0.2f;
            }
            if (bearX > range.y) {
                bearX = range.y; dir = direction::left; velocity = 0.0f; turnCooldown = 0.2f;
            }
        }

        // -------- Phase4-Up --------
        else {
            static bool isJumping = false;
            static float velocityY = 0.0f;
            static float jumpCooldown = 0.0f;

            const float gravity = -2000.0f;
            const float jumpPower = 1385.0f;  // 高度可達 332
            const float groundY = -120.0f;   // Moved up from -148.0f

            if (!isJumping && jumpCooldown <= 0.0f) {
                isJumping = true;
                velocityY = jumpPower;
            }

            if (isJumping) {
                velocityY += gravity * deltaTime;
                bearY += velocityY * deltaTime;

                bearX += (playerPosition.x - bearX) * 0.07f;
                dir = (playerPosition.x < bearX) ? direction::left : direction::right;

                if (bearY <= groundY) {
                    bearY = groundY;
                    isJumping = false;
                    jumpCooldown = 0.3f;
                }
            } else {
                bearY = groundY;
                jumpCooldown -= deltaTime;
            }
        }
    }

    // === Phase5 ===
    else if (m_CurrentPhase == "5") {
        static bool toLeft = true;
        const float speed = 2.5f;
        const float minX = -480.0f;
        const float maxX = 420.0f;

        if (toLeft) {
            bearX -= speed;
            dir = direction::left;
            if (bearX <= minX) toLeft = false;
        } else {
            bearX += speed;
            dir = direction::right;
            if (bearX >= maxX) toLeft = true;
        }
        bearY = 360.0f;  // Moved up from 332.0f
    }
}
