#ifndef MAPINFOLOADER_HPP
#define MAPINFOLOADER_HPP

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class MapInfoLoader {
public:
    explicit MapInfoLoader(int initialMap = 0);

    [[nodiscard]] const std::vector<std::vector<int>>& GetMapData() const { return m_MapData; }
    [[nodiscard]] int GetTile(int x, int y) const;
    [[nodiscard]] int GetWidth() const { return m_Width; }
    [[nodiscard]] int GetHeight() const { return m_Height; }

    void NextMap();
    void LoadMap(int mapNumber); // 直接載入地圖數字

private:
    inline std::string ImagePath(int mapNumber) {
        return RESOURCE_DIR "/Image/Background/Map" + std::to_string(mapNumber) + ".txt";
    }

    std::vector<std::vector<int>> m_MapData;
    int m_CurrentMap;
    int m_Width = 0;
    int m_Height = 0;
};

#endif //MAPINFOLOADER_HPP
