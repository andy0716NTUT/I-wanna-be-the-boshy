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
    bool m_NeedsSave = false; // 標記是否需要執行延遲保存
    float m_SaveDelay = 0.0f; // 保存延遲計時器
      // 新增：子彈發射延遲相關
    bool m_BulletShotPending = false; // 標記是否有待發射的子彈
    float m_BulletShootDelay = 0.0f; // 子彈發射延遲計時器

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
    }    void play() {
        m_Drawable = m_Animation;
        m_Animation->Play();
        m_HasShot = true; // 標記已經發射過子彈
        m_NeedsSave = true; // 標記需要延遲儲存
        m_SaveDelay = 0.0f; // 重置延遲計時器
        
        // 新增：啟動1秒子彈發射延遲
        m_BulletShotPending = true;
        m_BulletShootDelay = 0.0f;
        // 不再記錄觸發時的位置，因為我們要使用發射時的实时位置
    }glm::vec2 GetPosition(){return m_Transform.translation ;}
    void SetPosition(glm::vec2 position) {
        m_Transform.translation = position;
    }
    
    // 設定目標位置（玩家位置）
    void SetTargetPosition(const glm::vec2& targetPos) {
        m_TargetPosition = targetPos;
    }    // 檢查是否需要發射子彈
    bool ShouldShootBullet() const {
        return m_BulletShotPending && m_BulletShootDelay >= 1.0f; // 1秒後發射
    }// 重置射擊狀態
    void ResetShotStatus() {
        m_HasShot = false;
        m_BulletShotPending = false;
        m_BulletShootDelay = 0.0f;
    }    // 取得朝向玩家的方向（使用实时玩家位置）
    Character::direction GetDirectionToTarget() const {
        return (m_TargetPosition.x < m_Transform.translation.x) ? 
               Character::direction::LEFT : Character::direction::RIGHT;
    }
    
    // 更新延遲儲存計時器
    bool UpdateSaveDelay(float deltaTime, glm::vec2& outSavePosition) {
        if (!m_NeedsSave) return false;
        
        m_SaveDelay += deltaTime;
        
        const float SAVE_DELAY_TIME = 0; // 延遲0.25秒保存
        if (m_SaveDelay >= SAVE_DELAY_TIME) {
            m_NeedsSave = false; // 重置標記
            outSavePosition = m_TargetPosition; // 設定保存位置
            return true; // 可以保存了
        }
        
        return false; // 還不能保存
    }
      // 重置延遲儲存狀態
    void ResetSaveStatus() {
        m_NeedsSave = false;
    }
    
    // 更新子彈發射延遲計時器
    void UpdateBulletShootDelay(float deltaTime) {
        if (m_BulletShotPending) {
            m_BulletShootDelay += deltaTime;
        }
    }
    
    // 從地圖創建檢查點
    static std::vector<std::shared_ptr<CheckPoint>> CreateFromMap(
       const std::shared_ptr<MapInfoLoader>& mapLoader,
       Util::Renderer& rootRenderer);
};
#endif //CHECKPOINT_HPP
