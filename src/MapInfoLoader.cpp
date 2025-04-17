#include "MapInfoLoader.hpp"


void MapInfoLoader::LoadMap(std::string mapNumber) {
    this->CurrentPhase = mapNumber ;
    std::string filePath = ImagePath(mapNumber);
    std::ifstream file(filePath);

    if (!file) {
        std::cerr << "Error: Cannot open map file: " << filePath << std::endl;
        return;
    }

    // **清空舊的地圖資料**
    m_MapData.clear();
    m_Width = 0;
    m_Height = 0;

    std::string line;
    while (std::getline(file, line)) {
        std::vector<int> row;
        std::stringstream ss(line);
        int value;

        while (ss >> value) {
            row.push_back(value);
            if (ss.peek() == ',') ss.ignore(); // 處理 CSV 格式（可選）
        }

        if (row.empty()) continue; // 跳過空行
        if (m_Width == 0) m_Width = row.size(); // 設定地圖寬度
        m_MapData.push_back(row);
    }

    m_Height = m_MapData.size(); // 設定地圖高度
    file.close();

    std::cout << "Loaded Map " << mapNumber << " (" << m_Width << "x" << m_Height << ")" << std::endl;
}

int MapInfoLoader::GetTile(int x, int y) const {
    if (y < 0 || y >= m_Height || x < 0 || x >= m_Width) return 0; // 超出範圍當作空氣
    return m_MapData[y][x];
}
