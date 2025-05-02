#ifndef PHASE2TRAP_HPP
#define PHASE2TRAP_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Renderer.hpp"
#include "MapInfoLoader.hpp"

class phase2trap : public Util::GameObject {
public:
    phase2trap();

    void Create(std::shared_ptr<MapInfoLoader> MapInfoLoader,const std::string& imagePath, glm::vec2 position, float moveRange);
    void Update(float deltaTime);
    void clear();
    void SetActive(bool active);
    static void ResetFrameFlag();
    void LoadTrapData(const std::string& filePath);

private:
    std::shared_ptr<Util::Image> m_Image;
    std::shared_ptr<MapInfoLoader> m_MapInfoLoader;
    std::vector<std::vector<int>> m_trapMapData;
    std::vector<std::vector<int>> m_originalTiles;
    float m_InitialY;
    float m_MoveRange = 240.0f;
    bool isActivate = true;
    float m_Timer = 0.0f;  // 記錄時間
    std::set<std::pair<int, int>> m_LastOccupiedTiles;
};

#endif //PHASE2TRAP_HPP
