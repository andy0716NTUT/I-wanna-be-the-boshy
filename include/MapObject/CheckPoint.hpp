//
// Created by andyl on 2025/4/12.
//

#ifndef CHECKPOINT_HPP
#define CHECKPOINT_HPP
#include "Util/Animation.hpp"
#include "Util/GameObject.hpp"
#include "MapInfoLoader.hpp"
#include "Util/Renderer.hpp"
#include "Character.hpp"

// 前置宣告，避免循環引用
class Bullet;

class CheckPoint : public Util::GameObject{
private:
    std::shared_ptr<Util::Animation> m_Animation;
    std::vector<std::string> paths;
    bool m_HasShot = false; // 標記是否已經發射過子彈
    glm::vec2 m_TargetPosition = {0, 0}; // 玩家位置

public:
    explicit CheckPoint() {
        std::vector<std::string> frames;
        for (int i = 1; i <= 13; ++i) {
            frames.push_back(RESOURCE_DIR"/Image/CheckPoint/Checkpoint" + std::to_string(i) + ".png");
        }
        this->paths = frames;
        m_Animation = std::make_shared<Util::Animation>(frames, true, 100, false);
        m_Animation->Pause();
        m_Drawable = m_Animation;
    }
    
    void play() {
        m_Drawable = m_Animation;
        m_Animation->Play();
        m_HasShot = true; // 標記已經發射過子彈
    }    glm::vec2 GetPosition(){return m_Transform.translation ;}
    void SetPosition(glm::vec2 position) {
        m_Transform.translation = position;
    }
    
    // 設定目標位置（玩家位置）
    void SetTargetPosition(const glm::vec2& targetPos) {
        m_TargetPosition = targetPos;
    }
      // 檢查是否需要發射子彈
    bool ShouldShootBullet() const {
        return m_HasShot && m_Animation->GetCurrentFrameIndex() == 7; // 在動畫中間發射
    }
    
    // 重置射擊狀態
    void ResetShotStatus() {
        m_HasShot = false;
    }
    
    // 取得朝向玩家的方向
    Character::direction GetDirectionToTarget() const {
        return (m_TargetPosition.x < m_Transform.translation.x) ? 
               Character::direction::LEFT : Character::direction::RIGHT;
    }
    
    // 從地圖創建檢查點
    static std::vector<std::shared_ptr<CheckPoint>> CreateFromMap(
       const std::shared_ptr<MapInfoLoader>& mapLoader,
       Util::Renderer& rootRenderer);
};
#endif //CHECKPOINT_HPP
