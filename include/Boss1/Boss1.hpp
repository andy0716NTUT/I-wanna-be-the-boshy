//
// Created by andyl on 2025/5/3.
//

#include "BossHpInfo.hpp"
#include "Util/GameObject.hpp"
#include "Util/Animation.hpp"
#include "Util/Renderer.hpp"
#include "BulletTypeA.hpp"
#include "BulletTypeB.hpp"
#include "BulletTypeC.hpp"
#include "LightAttack.hpp"
#include "Boss1/BossDeadAnimation.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "MapInfoLoader.hpp"
    class Boss1 : public Util::GameObject{

    public:
        enum class AttackType {
            SPAWN,
            TYPEA,
            LIGHTATTACK,
            TYPEB,
            TYPEC,
            DEAD,
        };
        Boss1();
        void Spawn(float deltaType,Util::Renderer& rootRenderer);

        void Update(float deltaTime,glm::vec2 playerPosition, Util::Renderer& rootRenderer);

        void SetHealth(float damage);

        float GetHealth();

        void TakeDamage(int damage);

        glm::vec2 GetPosition();

        bool playerDead();

        bool Boss1Finished();

        bool IsDead() const;

        void ClearAnimation(Util::Renderer& rootRenderer);

        void CheckPlayerDeathByBullets(const glm::vec2& playerPosition);
    private:
        int maxHp = 50;
        int currentHp = 50;
        bool m_CanMoveVertically = true;
        bool isPlayerDead = false;
        float Health = 100;
        const int PositionX = 500;
        float m_Angle = 0.0f;
        float m_SpawnY = -300.0f;
        float m_WaitTimer = 0.0f;
        float m_ShootTimer = 0.0f;


        bool DeadAnimationFinished = false;

        std::shared_ptr<BossHpInfo> m_HP;

        AttackType m_AttackType = AttackType::SPAWN;
        std::shared_ptr<Util::Animation> m_Animation;
        std::shared_ptr<BossDeadAnimation> m_DeadAnimation;

        int TypeAShootCount = 0;
        std::vector<std::shared_ptr<BulletTypeA>> m_BulletsA;

        std::shared_ptr<LightAttack> m_LightAttack;
        float m_LightAttackTimer = 0.0f;      // 計時器
        int m_LightAttackCount = 0;           // 已射擊光束數量
        bool m_HasStartedLightAttack = false; // 是否開始計時

        std::vector<std::shared_ptr<BulletTypeB>> m_BulletsB;
        float m_ShootTimerB = 0.0f;
        int TypeBShootCount = 0 ;

        float TypeCFireInterval = 0.1f;  // 發射間隔（密集度）
        float TypeCFireDuration = 7.0f;// 發射持續時間
        float m_ShootTimerC = 0.0f;
        float m_ElapsedTimeC = 0.0f;
        float TypeC_Amplitude = 100.0f;   // 波浪振幅
        float TypeC_Frequency = 6.0f;     // 波浪頻率
        std::vector<std::shared_ptr<BulletTypeC>> m_BulletsC;

    };


