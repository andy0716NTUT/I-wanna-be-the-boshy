#include "../tinyxml2/tinyxml2.h"


#include "ParserBackground.hpp"
#include <iostream>
#include <sstream>

ParserBackground::ParserBackground(const std::string& filePath) : filePath(filePath) {}

bool ParserBackground::load() {
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(filePath.c_str()) != tinyxml2::XML_SUCCESS) {
        std::cerr << "Failed to load TMX file: " << filePath << std::endl;
        return false;
    }

    // 取得 map 標籤
    tinyxml2::XMLElement* mapElement = doc.FirstChildElement("map");
    if (!mapElement) {
        std::cerr << "No <map> tag found in TMX!" << std::endl;
        return false;
    }

    // 解析圖層
    tinyxml2::XMLElement* layerElement = mapElement->FirstChildElement("layer");
    while (layerElement) {
        TileLayer layer;
        const char* name;
        if (layerElement->QueryStringAttribute("name", &name) == tinyxml2::XML_SUCCESS) {
            layer.name = name; // 轉換為 std::string
        }
        layerElement->QueryIntAttribute("width", &layer.width);
        layerElement->QueryIntAttribute("height", &layer.height);

        // 解析 <data> 內的數據
        tinyxml2::XMLElement* dataElement = layerElement->FirstChildElement("data");
        if (dataElement && dataElement->GetText()) {
            std::istringstream stream(dataElement->GetText());
            int tile;
            char comma;
            while (stream >> tile) {
                layer.tiles.push_back(tile);
                stream >> comma; // 跳過逗號
            }
        }

        layers.push_back(layer);
        layerElement = layerElement->NextSiblingElement("layer");
    }

    return true;
}

void ParserBackground::printLayerData() const {
    for (const auto& layer : layers) {
        std::cout << "Layer: " << layer.name << " (" << layer.width << "x" << layer.height << ")\n";
        int count = 0;
        for (int tile : layer.tiles) {
            std::cout << tile << " ";
            if (++count % layer.width == 0) std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}
