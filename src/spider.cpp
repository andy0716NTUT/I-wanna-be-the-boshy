//
// Created by andyl on 2025/5/16.
//

#include "../include/MapObject/spider.hpp"
spider::spider() {
    std::vector<std::string> frame = {
        RESOURCE_DIR"/Image/enemy/spider1.png",
        RESOURCE_DIR"/Image/enemy/spider2.png"
    };
    m_animation = std::make_shared<Util::Animation>(frame, false, 100, true);
    m_Transform.translation = {300.0f, 800.0f};
    m_animation->Pause();
    SetDrawable(m_animation);
}
void spider::reset() {
    this->exist = false;
}

bool spider::detect(glm::vec2 position) {
    if (position.x >= -100.0f && position.y < 0 ) {
        m_animation->Play();
        this->exist = true;
        m_State = State::FALLING;
        return true;
    }
    return false;
}

void spider::RestoreAllTiles() {
    if (!m_MapInfoLoader) return;
    for (const auto& tilePos : m_LastOccupiedTiles) {
        int mapX = tilePos.first;
        int mapY = tilePos.second;

        if (mapX >= 0 && mapX < m_MapInfoLoader->GetWidth() &&
            mapY >= 0 && mapY < m_MapInfoLoader->GetHeight()) {
            m_MapInfoLoader->SetTile(mapX, mapY, m_originalTiles[mapY][mapX]);
            }
    }
    m_LastOccupiedTiles.clear();
}
void spider::Update(float deltaTime) {
    if (!exist || !m_MapInfoLoader) return;

    if (!m_tilesInitialized) {
        m_trapMapData1 = MapInfoLoader::LoadTileData(RESOURCE_DIR"/Image/enemy/spider1.txt");
        m_trapMapData2 = MapInfoLoader::LoadTileData(RESOURCE_DIR"/Image/enemy/spider2.txt");
        m_originalTiles = m_MapInfoLoader->GetMapData();
        m_tilesInitialized = true;
    }


    const std::vector<std::vector<int>>* currentMask = nullptr;
    if (m_State == State::FALLING || m_State == State::RISING) {
        currentMask = &m_trapMapData1;
    } else if (m_State == State::DIVE) {
        currentMask = &m_trapMapData2;
    }

    std::set<std::pair<int, int>> currentOccupiedTiles;

    if (currentMask && m_animation) {
        float imageCenterX = m_Transform.translation.x;
        float imageCenterY = m_Transform.translation.y;
        float imageHalfWidth = m_animation->GetSize().x / 2.0f;
        float imageHalfHeight = m_animation->GetSize().y / 2.0f;

        int baseTileX = static_cast<int>((imageCenterX - imageHalfWidth + 640) / 16);
        int baseTileY = static_cast<int>((480 - (imageCenterY + imageHalfHeight)) / 16);

        for (size_t y = 0; y < currentMask->size(); ++y) {
            for (size_t x = 0; x < (*currentMask)[y].size(); ++x) {
                if ((*currentMask)[y][x] != 5) continue;

                int mapX = baseTileX + static_cast<int>(x);
                int mapY = baseTileY + static_cast<int>(y);

                if (mapX < 0 || mapX >= m_MapInfoLoader->GetWidth() ||
                    mapY < 0 || mapY >= m_MapInfoLoader->GetHeight())
                    continue;

                float tileWorldLeftX = mapX * 16 - 640;
                float tileWorldTopY = 480 - mapY * 16;
                float tileCenterX = tileWorldLeftX + 8;
                float tileCenterY = tileWorldTopY - 8;

                if (std::abs(tileCenterX - imageCenterX) <= imageHalfWidth &&
                    std::abs(tileCenterY - imageCenterY) <= imageHalfHeight) {
                    m_MapInfoLoader->SetTile(mapX, mapY, 5);
                    currentOccupiedTiles.insert({mapX, mapY});
                }
            }
        }

        // 還原 tiles
        for (const auto& tilePos : m_LastOccupiedTiles) {
            if (currentOccupiedTiles.find(tilePos) == currentOccupiedTiles.end()) {
                int mapX = tilePos.first;
                int mapY = tilePos.second;
                m_MapInfoLoader->SetTile(mapX, mapY, m_originalTiles[mapY][mapX]);
            }
        }

        m_LastOccupiedTiles = currentOccupiedTiles;
        m_Transform.translation.x = std::round(m_Transform.translation.x);
        m_Transform.translation.y = std::round(m_Transform.translation.y);
    }

    // 🎮 動畫狀態機
    switch (m_State) {
        case State::FALLING:
            m_Transform.translation.y -= 25.0f;
            if (m_Transform.translation.y <= -400.0f) {
                m_State = State::RISING;
            }
            break;

        case State::RISING:
            m_Transform.translation.y += 25.0f;
            if (m_Transform.translation.y >= 600.0f) {
                m_State = State::DIVE_WAIT;
                m_Timer = 1.0f;  // 停一秒
            }
            break;

        case State::DIVE_WAIT:
            m_Timer -= deltaTime;
            if (m_Timer <= 0.0f) {
                m_State = State::DIVE;
            }
            break;

        case State::DIVE:
            m_Transform.translation.y -= 100.0f;
            if (m_Transform.translation.y < -850.0f) {
                m_State = State::FINISHED;
            }
            break;

        case State::FINISHED:
            // 🧹 清除碰撞 tiles，還原地圖
            for (int y = 0; y < 60; ++y) {
                for (int x = 0; x < 80; ++x) {
                    m_MapInfoLoader->SetTile(x, y, m_originalTiles[y][x]);
                }
            }
            this->SetVisible(false);
            this->SetDrawable(nullptr);
            RestoreAllTiles();
        default:
            break;
    }
}
bool spider::IsFinished() const {
    return m_State == State::FINISHED;
}

