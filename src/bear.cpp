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
    if ((phase == "4" || phase.find("4_") != std::string::npos) && !this->spawn) {
        this->spawn = true;
        this->attack = AttackType::CHASE;
        // Set the bear's respawn position
        this->SetPosition(glm::vec2(275, -135));
        
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
    } else if (phase == "5") {
        this->attack = AttackType::ROUND;
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
    // Update the drawable based on the bear's direction
    if (dir == direction::left) {
        m_Drawable = m_Animation_left;
    } else {
        m_Drawable = m_Animation_right;
    }

    // Determine behavior based on attack type
    switch (this->attack) {
        case AttackType::CHASE:
            if (m_Transform.translation.x - playerPosition.x > 0 )this->dir = direction::left;
            if (m_Transform.translation.x - playerPosition.x < 0)this->dir = direction::right;
            // Apply floor constraints only for phase "4_"
            if (m_CurrentPhase.find("4_") != std::string::npos && m_MapInfoLoader) {
                // Apply gravity
                m_velocityY += m_Gravity;
                if (m_velocityY < m_MaxFallSpeed) {
                    m_velocityY = m_MaxFallSpeed;
                }
                
                // Calculate tile position (same as Boshy's calculation)
                int tileX = static_cast<int>((m_Transform.translation.x + 640) / 16);
                int tileY = static_cast<int>((480 - m_Transform.translation.y) / 16);
                
                // Ensure tileX and tileY are within bounds
                if (tileX < 0) tileX = 0;
                if (tileX >= m_MapInfoLoader->GetWidth()) tileX = m_MapInfoLoader->GetWidth() - 1;
                if (tileY < 0) tileY = 0;
                if (tileY >= m_MapInfoLoader->GetHeight()) tileY = m_MapInfoLoader->GetHeight() - 1;
                  // Get tiles around the bear (just like Boshy does)
                int belowTile = m_MapInfoLoader->GetTile(tileX, tileY + 1);
                int currentTile = m_MapInfoLoader->GetTile(tileX, tileY);
                
                // Handle floor collision exactly like Boshy
                if ((belowTile == 1 || belowTile == 8) && m_velocityY < 0) {
                    // Adjust position to sit on top of the floor with same compensation as Boshy (-4)
                    m_Transform.translation.y = 480 - ((tileY) * 16) - 4;
                    m_velocityY = 0;
                    m_onGround = true;
                } else if (currentTile == 1 && m_velocityY >= 0) {
                    // This handles collisions when falling into a platform from above
                    m_Transform.translation.y = 480 - (tileY - 1) * 16;
                    m_velocityY = 0;
                    m_onGround = true;
                } else {
                    // Apply gravity effect
                    m_Transform.translation.y += m_velocityY;
                    m_onGround = false;
                }
                  // Only allow horizontal movement when on ground
                if (m_onGround) {
                    // Only track player's X position, ignore Y position completely
                    if (m_Transform.translation.x - playerPosition.x > 0) {
                        // Player is to the left of the bear
                        this->dir = direction::left;
                        // Move left towards the player
                        m_Transform.translation.x -= 3.0f;  // Adjust speed as needed
                    } else {
                        // Player is to the right of the bear
                        this->dir = direction::right;
                        // Move right towards the player
                        m_Transform.translation.x += 3.0f;  // Adjust speed as needed
                    }
                }
            } else {                // Regular chase behavior for other phases - only track X position
                if (m_Transform.translation.x - playerPosition.x > 0) {
                    // Player is to the left of the bear
                    this->dir = direction::left;
                    // Move left towards the player
                    m_Transform.translation.x -= 3.0f;  // Adjust speed as needed
                } else {
                    // Player is to the right of the bear
                    this->dir = direction::right;
                    // Move right towards the player
                    m_Transform.translation.x += 3.0f;  // Adjust speed as needed
                }
                
                // No vertical movement - maintain current Y position
            }
            break;case AttackType::ROUND:
            // Implement circular movement pattern
            static float angle = 0.0f;
            static float radius = 150.0f;  // Adjust the circle radius as needed
            static glm::vec2 centerPosition;
            
            // Initialize the center position only once when transitioning to ROUND attack
            static bool centerInitialized = false;
            if (!centerInitialized) {
                centerPosition = m_Transform.translation;
                centerInitialized = true;
            }

            // Increase the angle for circular motion
            angle += 0.03f;  // Adjust speed of rotation as needed
            
            // Calculate new position using parametric equation of a circle
            m_Transform.translation.x = centerPosition.x + radius * cos(angle);
            m_Transform.translation.y = centerPosition.y + radius * sin(angle);
            
            // Update the bear's direction based on movement
            if (cos(angle) < 0) {
                this->dir = direction::left;
            } else {
                this->dir = direction::right;
            }
            break;
    }
}

