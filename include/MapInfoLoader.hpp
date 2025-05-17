#ifndef MAPINFOLOADER_HPP
#define MAPINFOLOADER_HPP

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class MapInfoLoader {
public:
    [[nodiscard]] const std::vector<std::vector<int>>& GetMapData() const { return m_MapData; }
    [[nodiscard]] int GetTile(int x, int y) const;
    [[nodiscard]] int GetWidth() const { return m_Width; }
    [[nodiscard]] int GetHeight() const { return m_Height; }

    void LoadMap(std::string mapNumber,std::string CurrentWorld); // 直接載入地圖數字

    static std::vector<std::vector<int>> LoadTileData(const std::string& path);

    void SetTile(int x, int y, int value); // 新增此方法

    std::string GetCurrentPhase() { return this->CurrentPhase; }

    std::vector<std::vector<int>> GetMapDate() {
        return this->m_MapData;
    }


private:
    std::string CurrentPhase;
    inline std::string ImagePath(const std::string mapNumber, std::string CurrentWorld) {
        return RESOURCE_DIR "/Image/Background/" + CurrentWorld +"/Map" + mapNumber + ".txt";
    }

    std::vector<std::vector<int>> m_MapData;
    int m_Width = 0;
    int m_Height = 0;
};

#endif // MAPINFOLOADER_HPP