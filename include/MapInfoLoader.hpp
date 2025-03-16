//
// Created by andyl on 2025/3/8.
//

#ifndef MAPINFOLOADER_HPP
#define MAPINFOLOADER_HPP

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class MapInfoLoader {
public:
    explicit MapInfoLoader(const std::string& filePath);
    [[nodiscard]] const std::vector<std::vector<int>>& GetMapData() const { return m_MapData; }
    [[nodiscard]] int GetTile(int x, int y) const;
    [[nodiscard]] int GetWidth() const { return m_Width; }
    [[nodiscard]] int GetHeight() const { return m_Height; }

private:
    std::vector<std::vector<int>> m_MapData;
    int m_Width = 0;
    int m_Height = 0;
    void LoadMap(const std::string& filePath);
};

#endif //MAPINFOLOADER_HPP
