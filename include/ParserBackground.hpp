#ifndef PARSERBACKGROUND_HPP
#define PARSERBACKGROUND_HPP

#include "../tinyxml2/tinyxml2.h"
#include <string>
#include <vector>

struct TileLayer {
    std::string name;
    std::vector<int> tiles;
    int width, height;
};

class ParserBackground {
public:
    ParserBackground(const std::string& filePath);
    bool load(); // 解析地圖
    void printLayerData() const; // 測試輸出層數據

private:
    std::string filePath;
    std::vector<TileLayer> layers;
};

#endif // PARSERBACKGROUND_HPP
