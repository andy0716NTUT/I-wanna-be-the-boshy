#include "MapObject/phase2trap.hpp"
#include <cmath> // 加入 sin()
phase2trap::phase2trap() {}

void phase2trap::Create(std::shared_ptr<MapInfoLoader> MapInfoLoader,const std::string& imagePath, glm::vec2 position, float moveRange) {
    m_Image = std::make_shared<Util::Image>(imagePath + ".png");
    m_Drawable = m_Image;
    m_Transform.translation = position;
    m_InitialY = position.y;
    m_MoveRange = moveRange;
    m_Timer = 0.0f;
    isActivate = true;
    this->m_MapInfoLoader = MapInfoLoader;
    this->LoadTrapData(imagePath + ".txt");
    this->m_originalTiles = m_MapInfoLoader->GetMapData();
}

void phase2trap::Update(float deltaTime) {
    if (!isActivate) return;

    // 更新動畫位置
    m_Timer += deltaTime;
    m_Transform.translation.y = m_InitialY + std::sin(m_Timer * 2.0f) * m_MoveRange;

    // 計算圖片中心座標
    float imageCenterX = m_Transform.translation.x;
    float imageCenterY = m_Transform.translation.y;

    // 計算圖片左右上下範圍
    float imageHalfWidth = m_Image->GetSize().x / 2.0f;
    float imageHalfHeight = m_Image->GetSize().y / 2.0f;

    // 計算圖片左上角對應的 tile 座標（tile 大小16）
    int baseTileX = static_cast<int>((imageCenterX - imageHalfWidth + 640) / 16);
    int baseTileY = static_cast<int>((480 - (imageCenterY + imageHalfHeight)) / 16);

    std::set<std::pair<int,int>> currentOccupiedTiles;

    for (size_t y = 0; y < m_trapMapData.size(); ++y) {
        for (size_t x = 0; x < m_trapMapData[y].size(); ++x) {
            if (m_trapMapData[y][x] != 5) continue; // 只處理有 5 的 tile

            int mapX = baseTileX + static_cast<int>(x);
            int mapY = baseTileY + static_cast<int>(y);

            if (mapX < 0 || mapX >= m_MapInfoLoader->GetWidth() ||
                mapY < 0 || mapY >= m_MapInfoLoader->GetHeight())
                continue;

            // 計算 tile 中心的世界座標
            float tileWorldLeftX = mapX * 16 - 640;
            float tileWorldTopY = 480 - mapY * 16;
            float tileCenterX = tileWorldLeftX + 8;
            float tileCenterY = tileWorldTopY - 8;

            // 判斷 tile 中心是否在圖片範圍內
            if (std::abs(tileCenterX - imageCenterX) <= imageHalfWidth &&
                std::abs(tileCenterY - imageCenterY) <= imageHalfHeight) {
                m_MapInfoLoader->SetTile(mapX, mapY, 5);
                currentOccupiedTiles.insert({mapX, mapY});
            }
        }
    }

    // 還原「上一次有佔據但這次沒有佔據」的 tile
    for (const auto& tilePos : m_LastOccupiedTiles) {
        if (currentOccupiedTiles.find(tilePos) == currentOccupiedTiles.end()) {
            int mapX = tilePos.first;
            int mapY = tilePos.second;
            m_MapInfoLoader->SetTile(mapX, mapY, m_originalTiles[mapY][mapX]);
        }
    }
    m_Transform.translation.x = std::round(m_Transform.translation.x);
    m_Transform.translation.y = std::round(m_Transform.translation.y);
    // 更新 lastOccupiedTiles
    m_LastOccupiedTiles = currentOccupiedTiles;
}



void phase2trap::LoadTrapData(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file) {
        std::cerr << "Failed to load trap map: " << filePath << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::vector<int> row;
        std::stringstream ss(line);
        int value;
        while (ss >> value) {
            row.push_back(value);
            if (ss.peek() == ',') ss.ignore(); // skip comma
        }
        m_trapMapData.push_back(row);
    }
    file.close();
}

void phase2trap::clear() {
    m_Drawable = nullptr;
    m_Image = nullptr;
    isActivate = false;
}

void phase2trap::SetActive(bool active) {
    isActivate = active;
}
