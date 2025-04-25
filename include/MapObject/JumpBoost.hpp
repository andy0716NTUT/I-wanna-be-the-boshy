#ifndef JUMPBOOST_HPP
#define JUMPBOOST_HPP
#include "MapInfoLoader.hpp"
#include "Util/Animation.hpp"
#include "Util/GameObject.hpp"


class JumpBoost : public Util::GameObject{
private:
    std::shared_ptr<Util::Animation> m_Animation;
    std::vector<std::string> paths;
    bool Visible = true;
    float m_Timer = 0.0f;    // 重现计时器
    const float m_ReappearTime = 3.0f;  // 消失后重现的时间
    
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
    
    void SetDisappear(bool dis){this->Visible = dis;}
    
    bool GetDisapper(){return this->Visible;}
    
    // 检查角色是否触碰到JumpBoost，如果是则消耗一次跳跃次数并隐藏JumpBoost
    bool CheckInteraction(const glm::vec2& playerPosition, int& jumpCount) {
        if (glm::distance(GetPosition(), playerPosition) < 20.0f) {
            if (jumpCount >= 1) {
                jumpCount--;
                SetDisappear(false);
                SetVisible(GetDisapper());
                return true;
            }
        }
        return false;
    }
    
    // 更新JumpBoost状态，包括隐藏后的重现计时
    void UpdateState(float deltaTime) {
        if (!GetDisapper()) {
            m_Timer += deltaTime;
            if (m_Timer > m_ReappearTime) {
                SetVisible(true);
                SetDisappear(true);
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