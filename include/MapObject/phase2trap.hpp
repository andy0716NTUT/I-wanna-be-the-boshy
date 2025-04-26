#ifndef PHASE2TRAP_HPP
#define PHASE2TRAP_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Renderer.hpp"
#include "MapInfoLoader.hpp"

class phase2trap : public Util::GameObject {
public:
    phase2trap();

    void Create(const std::string& imagePath, glm::vec2 position, float moveRange);
    void Update(float deltaTime);
    void clear();
    void SetActive(bool active);
    static void ResetFrameFlag();

private:
    std::shared_ptr<Util::Image> m_Image;
    float m_InitialY;
    float m_MoveRange = 240.0f;
    bool isActivate = true;
    float m_Timer = 0.0f;  // 記錄時間
    std::vector<std::pair<int, int>> m_LastOccupiedTiles;
};

#endif //PHASE2TRAP_HPP
