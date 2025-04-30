#ifndef JUMPBOOST_HPP
#define JUMPBOOST_HPP
#include "MapInfoLoader.hpp"
#include "spdlog/fmt/bundled/chrono.h"
#include "Util/Animation.hpp"
#include "Util/GameObject.hpp"


class JumpBoost : public Util::GameObject{
private:
    std::shared_ptr<Util::Animation> m_Animation;
    std::vector<std::string> paths;
    bool m_IsActive = true;
    bool m_Disappear = false; // ➤ true: 已消失, false: 顯示中
    float m_Timer = 0.0f;
    const float m_ReappearTime = 3.0f;
    
public:
    explicit JumpBoost() {
        std::vector<std::string> frames;
        for (int i = 1; i <= 2; ++i) {
            frames.push_back(RESOURCE_DIR"/Image/DJ" + std::to_string(i) + ".png");
        }
        m_Animation = std::make_shared<Util::Animation>(frames, true, 300, true);
        m_Animation->Play();
        m_Drawable = m_Animation;
    }
    
    glm::vec2 GetPosition(){return m_Transform.translation;}
    
    void SetPosition(glm::vec2 position) {
        m_Transform.translation = position;
    }
    // 检查角色是否触碰到JumpBoost，如果是则消耗一次跳跃次数并隐藏JumpBoost
    bool CheckInteraction(const glm::vec2& playerPosition, int& jumpCount) {
        if (glm::distance(GetPosition(), playerPosition) < 32.0f && m_IsActive) {
            if (jumpCount >= 1) {
                jumpCount--;
                m_IsActive = false;
                m_Disappear = true;             // ✅ 標記為已消失
                m_Drawable = nullptr;
                m_Visible = false;
                return true;
            }
        }
        return false;
    }
    
    // 更新JumpBoost状态，包括隐藏后的重现计时
    void UpdateState(float deltaTime) {
        if (m_Disappear) {                       // ✅ 表示進入隱藏狀態
            m_Timer += deltaTime;
            if (m_Timer > m_ReappearTime) {
                m_IsActive = true;
                m_Disappear = false;            // ✅ 顯示狀態回復
                m_Visible = true;
                m_Drawable = m_Animation;
                m_Timer = 0.0f;
            }
        }
    }
    
    static std::vector<std::shared_ptr<JumpBoost>> CreateFromMap(
       const std::shared_ptr<MapInfoLoader>& mapLoader,
       Util::Renderer& rootRenderer) {
        std::vector<std::shared_ptr<JumpBoost>> jumpBoosts;

        for (int y = 0; y < mapLoader->GetHeight() - 1; ++y) {
            for (int x = 0; x < mapLoader->GetWidth() - 1; ++x) {
                if (mapLoader->GetTile(x, y) == 4 &&
                    mapLoader->GetTile(x + 1, y) == 4 &&
                    mapLoader->GetTile(x, y + 1) == 4 &&
                    mapLoader->GetTile(x + 1, y + 1) == 4) {

                    float worldX = (x + 1) * 16 - 640;
                    float worldY = 480 - (y + 1) * 16;

                    auto jumpBoost = std::make_shared<JumpBoost>();
                    jumpBoost->SetPosition({worldX, worldY});
                    jumpBoost->SetZIndex(-2);
                    rootRenderer.AddChild(jumpBoost);
                    jumpBoosts.push_back(jumpBoost);
                    }
            }
        }

        return jumpBoosts;
    }
};

#endif //JUMPBOOST_HPP