#include "MapInfoLoader.hpp"
#include <filesystem>

namespace {
std::vector<std::vector<int>> ReadMap(const std::string& path) {
    std::ifstream file(std::filesystem::u8path(path));
    if (!file) {
        std::cerr << "[MapInfoLoader] Cannot open map: " << path << '\n';
        return {};
    }
    std::vector<std::vector<int>> result;
    std::string line;
    std::size_t lineNumber = 0;
    while (std::getline(file, line)) {
        ++lineNumber;
        // Both comma-separated and whitespace-separated maps already exist.
        for (auto& ch : line) if (ch == ',') ch = ' ';
        std::stringstream stream(line);
        std::vector<int> row;
        int value;
        while (stream >> std::ws && !stream.eof()) {
            if (!(stream >> value)) {
                std::cerr << "[MapInfoLoader] Invalid integer in " << path
                          << " at line " << lineNumber << '\n';
                return {};
            }
            row.push_back(value);
        }
        if (row.empty()) continue; // Preserve original blank-line handling.
        if (!result.empty() && row.size() != result.front().size()) {
            std::cerr << "[MapInfoLoader] Inconsistent row width in " << path
                      << " at line " << lineNumber << ": expected "
                      << result.front().size() << ", got " << row.size() << '\n';
            return {};
        }
        result.push_back(std::move(row));
    }
    if (file.bad() || result.empty()) {
        std::cerr << "[MapInfoLoader] Empty or unreadable map: " << path << '\n';
        return {};
    }
    return result;
}
} // namespace

void MapInfoLoader::LoadMap(std::string mapNumber,std::string CurrentWorld) {
    this->CurrentPhase = mapNumber ;
    std::string filePath = ImagePath(mapNumber,CurrentWorld);
    m_MapData = ReadMap(filePath);
    m_Width = m_MapData.empty() ? 0 : static_cast<int>(m_MapData.front().size());
    m_Height = static_cast<int>(m_MapData.size());
    if (m_MapData.empty()) return;

    std::cout << "Loaded Map " << mapNumber << " (" << m_Width << "x" << m_Height << ")" << std::endl;
}

std::vector<std::vector<int>> MapInfoLoader::LoadTileData(const std::string& path) {
    return ReadMap(path);
}
int MapInfoLoader::GetTile(int x, int y) const {
    if (y >= 0 && y < m_Height && x >= 0 &&
        x < static_cast<int>(m_MapData[y].size())) {
        return m_MapData[y][x];
    }
    return -1; // 返回一個無效值，表示超出範圍
}
void MapInfoLoader::SetTile(int x, int y, int value) {
    if (y >= 0 && y < m_Height && x >= 0 &&
        x < static_cast<int>(m_MapData[y].size())) {
        m_MapData[y][x] = value;
    }
}
