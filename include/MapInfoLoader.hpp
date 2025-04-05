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

    void LoadMap(std::string mapNumber); // 直接載入地圖數字

private:
    inline std::string ImagePath(const std::string mapNumber) {
        return RESOURCE_DIR "/Image/Background/Map" + mapNumber + ".txt";
    }

    std::vector<std::vector<int>> m_MapData;
    int m_Width = 0;
    int m_Height = 0;
};

#endif //MAPINFOLOADER_HPP
