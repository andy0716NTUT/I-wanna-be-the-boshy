//
// Created by andyl on 2025/5/16.
//

#ifndef SPIDER_HPP
#define SPIDER_HPP
#include <memory>
#include <glm/vec2.hpp>

#include "Util/Animation.hpp"
#include "Util/GameObject.hpp"
#include "MapInfoLoader.hpp"

class spider : public Util::GameObject {
public:
    spider();
    bool detect(glm::vec2 position);
    void reset();
    void Update(float deltaTime);
    bool IsFinished() const;
    void SetMapLoader(std::shared_ptr<MapInfoLoader> loader) {
        m_MapInfoLoader = loader;
    }
    void RestoreAllTiles();
private:
    enum class State {
        NONE,
        FALLING,
        RISING,
        DIVE_WAIT,
        DIVE,
        FINISHED
    };
    std::vector<std::vector<int>> m_trapMapData1;
    std::vector<std::vector<int>> m_trapMapData2;
    std::vector<std::vector<int>> m_originalTiles;
    std::set<std::pair<int, int>> m_LastOccupiedTiles;
    std::shared_ptr<MapInfoLoader> m_MapInfoLoader;
    bool m_tilesInitialized = false;
    std::shared_ptr<Util::Animation> m_animation;
    State m_State = State::NONE;
    float m_Timer = 0.0f;
    bool exist = false;
};



#endif //SPIDER_HPP
