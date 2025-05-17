#include "MapInfoLoader.hpp"


void MapInfoLoader::LoadMap(std::string mapNumber,std::string CurrentWorld) {
    this->CurrentPhase = mapNumber ;
    std::string filePath = ImagePath(mapNumber,CurrentWorld);
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

std::vector<std::vector<int>> MapInfoLoader::LoadTileData(const std::string& path) {
    std::vector<std::vector<int>> result;
    std::ifstream file(path);

    if (!file.is_open()) {
        std::cerr << "[MapInfoLoader] Failed to open tile data file: " << path << std::endl;
        return result;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::vector<int> row;
        std::stringstream ss(line);
        std::string token;

        while (std::getline(ss, token, ',')) {
            try {
                row.push_back(std::stoi(token));
            } catch (...) {
                row.push_back(0); // 格式錯誤預設為0
            }
        }

        result.push_back(row);
    }

    file.close();
    return result;
}
int MapInfoLoader::GetTile(int x, int y) const {
    if (y >= 0 && y < m_Height && x >= 0 && x < m_Width) {
        return m_MapData[y][x];
    }
    return -1; // 返回一個無效值，表示超出範圍
}
void MapInfoLoader::SetTile(int x, int y, int value) {
    if (y >= 0 && y < m_Height && x >= 0 && x < m_Width) {
        m_MapData[y][x] = value;
    }
}
