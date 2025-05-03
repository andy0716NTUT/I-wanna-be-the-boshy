//
// Created by andyl on 2025/5/3.
//

#include "Util/GameObject.hpp"
#include "Util/Animation.hpp"
#include "Util/Renderer.hpp"
#include "BulletTypeA.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
    class Boss1 : public Util::GameObject{

    public:
        enum class AttackType {
            SPAWN,
            TYPEA,
            TYPEB,
            LIGHTATTACK,
            TYPEC,
        };
        Boss1();

        void Spawn(float deltaType);

        void Update(float deltaTime,glm::vec2 playerPosition, Util::Renderer& rootRenderer);

        void SetHealth(float damage);

        float GetHealth();

        glm::vec2 GetPosition();

    private:

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


    };


