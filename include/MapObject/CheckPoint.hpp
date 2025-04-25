//
// Created by andyl on 2025/4/12.
//

#ifndef CHECKPOINT_HPP
#define CHECKPOINT_HPP
#include "Util/Animation.hpp"
#include "Util/GameObject.hpp"
#include "MapInfoLoader.hpp"
#include "Util/Renderer.hpp"
class CheckPoint : public Util::GameObject{
private:
    std::shared_ptr<Util::Animation> m_Animation;
    std::vector<std::string> paths;
public:

    explicit CheckPoint() {
        std::vector<std::string> frames;
        for (int i = 1; i <= 13; ++i) {
            frames.push_back(RESOURCE_DIR"/Image/CheckPoint/Checkpoint" + std::to_string(i) + ".png");
        }
        m_Animation = std::make_shared<Util::Animation>(frames, true, 100, false);
        m_Animation->Pause();
        m_Drawable = m_Animation;
    }
    void play() {
        m_Drawable = m_Animation;
        m_Animation->Play();

    }
    glm::vec2 GetPosition(){return m_Transform.translation ;}
    void SetPosition(glm::vec2 position) {
        m_Transform.translation = position;
    }
    static std::vector<std::shared_ptr<CheckPoint>> CreateFromMap(
       const std::shared_ptr<MapInfoLoader>& mapLoader,
       Util::Renderer& rootRenderer);
};
#endif //CHECKPOINT_HPP
