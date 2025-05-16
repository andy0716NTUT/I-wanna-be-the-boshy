#include "Util/GameObject.hpp"
#include "Util/Animation.hpp"

class bear : public Util::GameObject {
public:
    enum class direction {
        left,
        right
    };
    enum class AttackType {
        CHASE,
        ROUND
    };
    bear();
    bool detect(std::string& phase);
    glm::vec2 GetPosition();
    void SetPosition(glm::vec2 position);
    bool exist();
    void Update(glm::vec2 playerPosition);
private:
    std::shared_ptr<Util::Animation> m_Animation_left;
    std::shared_ptr<Util::Animation> m_Animation_right;
    direction dir = direction::left;
    AttackType attack = AttackType::CHASE;
    bool spawn = false;
};