//
// Created by andyl on 2025/4/17.
//
#include "MapObject/CheckPoint.hpp"
#include "Bullet.hpp"
std::vector<std::shared_ptr<CheckPoint>> CheckPoint::CreateFromMap(
    const std::shared_ptr<MapInfoLoader>& mapLoader,
    Util::Renderer& rootRenderer) {

    std::vector<std::shared_ptr<CheckPoint>> checkpoints;

    for (int y = 0; y < mapLoader->GetHeight() - 1; ++y) {
        for (int x = 0; x < mapLoader->GetWidth() - 1; ++x) {
            if (mapLoader->GetTile(x, y) == 3 &&
                mapLoader->GetTile(x + 1, y) == 3 &&
                mapLoader->GetTile(x, y + 1) == 3 &&
                mapLoader->GetTile(x + 1, y + 1) == 3) {

                float worldX = (x + 1) * 16 - 640;
                float worldY = 480 - (y + 1) * 16;

                auto checkpoint = std::make_shared<CheckPoint>();
                checkpoint->SetPosition({worldX, worldY});
                rootRenderer.AddChild(checkpoint);
                checkpoints.push_back(checkpoint);
                }
        }
    }

    return checkpoints;
}