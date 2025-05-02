//
// Created by andyl on 2025/5/3.
//

#include "Util/GameObject.hpp"
#include "Util/Animation.hpp"

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

    void Update(float deltaTime);

    void SetHealth(float damage);

    float GetHealth();

    glm::vec2 GetPosition();

private:
    float Health = 100;
    const int PositionX = 500;

    float m_Angle = 0.0f;
    float m_SpawnY = -300.0f;
    float m_WaitTimer = 0.0f;


    AttackType m_AttackType = AttackType::SPAWN;
    std::shared_ptr<Util::Animation> m_Animation;

};

class BulletTypeA {
    public:
        BulletTypeA();
        void Shoot();
        void Update();
};

class BulletTypeB {
    public:
        BulletTypeB();
        void Shoot();
        void Update();
};

class BulletTypeC {
    public:
        BulletTypeC();
        void Shoot();
        void Update();
};

class LightAttack {
    public:
        LightAttack();
        void Shoot();
        void Update();
};