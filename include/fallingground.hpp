#ifndef FALLINGGROUND_HPP
#define FALLINGGROUND_HPP

#include <unordered_map>
#include <utility>
#include "MapInfoLoader.hpp"
#include "PairHash.hpp"

class FallingGround {
public:
    explicit FallingGround(MapInfoLoader* mapLoader);

    void Update(float deltaTime, int tileX, int tileY);
    void ResetTile(int tileX, int tileY);

private:
    MapInfoLoader* m_MapLoader;
    std::unordered_map<std::pair<int, int>, float, Util::PairHash> m_FallingTimers;
    const float m_FallingDelay = 1.0f; // 掉落延遲時間
    const int m_FallingTile = 7;       // 掉落地板的 Tile 值
};

#endif // FALLINGGROUND_HPP