#include "fallingground.hpp"

FallingGround::FallingGround(MapInfoLoader* mapLoader) : m_MapLoader(mapLoader) {}

void FallingGround::Update(float deltaTime, int tileX, int tileY) {
    // 更新掉落地板的邏輯
    for (auto it = m_FallingTimers.begin(); it != m_FallingTimers.end();) {
        auto& [tile, timer] = *it;
        timer -= deltaTime;
        if (timer <= 0.0f) {
            m_MapLoader->SetTile(tile.first, tile.second, 0); // 重置為空白
            it = m_FallingTimers.erase(it);
        } else {
            ++it;
        }
    }

    // 如果當前 Tile 是掉落地板，啟動計時器
    if (m_MapLoader->GetTile(tileX, tileY) == m_FallingTile) {
        m_FallingTimers[{tileX, tileY}] = m_FallingDelay;
    }
}

void FallingGround::ResetTile(int tileX, int tileY) {
    m_FallingTimers.erase({tileX, tileY});
    m_MapLoader->SetTile(tileX, tileY, m_FallingTile);
}