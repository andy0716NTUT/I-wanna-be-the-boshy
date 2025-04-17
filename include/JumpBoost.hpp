//
// Created by andyl on 2025/4/18.
//

#ifndef JUMPBOOST_HPP
#define JUMPBOOST_HPP
#include "MapInfoLoader.hpp"
#include "Util/Animation.hpp"
#include "Util/GameObject.hpp"


class JumpBoost : public Util::GameObject{
private:
    std::shared_ptr<Util::Animation> m_Animation;
    std::vector<std::string> paths;
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

