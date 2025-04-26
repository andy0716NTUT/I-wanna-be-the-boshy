#ifndef PHASE2TRAP_HPP
#define PHASE2TRAP_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Renderer.hpp"
#include "MapInfoLoader.hpp"
class phase2trap : public Util::GameObject {
public:
    phase2trap();

    void CheckCurrentPhase(const std::string& currentPhase); // 可以保留
    void SetImage(const std::string& Imagepath);
    void SetPosition(glm::vec2 position);
    void SetActive(bool active);
    void Update(float deltaTime);
    void clear();
    void Create(std::shared_ptr<MapInfoLoader> mapLoader, const std::string& imagePath, glm::vec2 position, const std::string& movingWay, float moveRange = 240.0f);
    bool CheckCollision(const glm::vec2& playerPos) const;

private:
    std::shared_ptr<MapInfoLoader> m_MapLoader;
    std::vector<std::pair<int, int>> m_LastOccupiedTiles;
    std::shared_ptr<Util::Image> m_Image;
    float m_MoveRange = 240.0f;
    bool isActivate = false;
    std::string MovingWay;
    float m_InitialY = 0.0f;
    float m_WaitTimer = 0.0f;   // 停留計時器
    bool m_IsWaiting = false;   // 是否正在停留
};

#endif // PHASE2TRAP_HPP