//
// Created by andyl on 2025/5/3.
//

#include "Util/GameObject.hpp"
#include "Util/Animation.hpp"
#include "Util/Renderer.hpp"
#include "BulletTypeA.hpp"
#include "LightAttack.hpp"
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
        };
        Boss1();
        void Spawn(float deltaType);

        void Update(float deltaTime,glm::vec2 playerPosition, Util::Renderer& rootRenderer);

        void SetHealth(float damage);

        float GetHealth();

        glm::vec2 GetPosition();

        bool playerDead();
    private:
        bool m_CanMoveVertically = true;
        bool isPlayerDead = false;
        float Health = 100;
        const int PositionX = 500;
        float m_Angle = 0.0f;
        float m_SpawnY = -300.0f;
        float m_WaitTimer = 0.0f;
        float m_ShootTimer = 0.0f;

        AttackType m_AttackType = AttackType::SPAWN;
        std::shared_ptr<Util::Animation> m_Animation;

        int TypeAShootCount = 0;
        std::vector<std::shared_ptr<BulletTypeA>> m_BulletsA;

        std::shared_ptr<LightAttack> m_LightAttack;
        float m_LightAttackTimer = 0.0f;      // 計時器
        int m_LightAttackCount = 0;           // 已射擊光束數量
        bool m_HasStartedLightAttack = false; // 是否開始計時

    };


