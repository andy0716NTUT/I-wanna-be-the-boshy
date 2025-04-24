#ifndef FALLINGGROUND_HPP
#define FALLINGGROUND_HPP

#include <unordered_map>
#include <utility>
#include "MapInfoLoader.hpp"
#include "Util/Image.hpp"
#include "Util/GameObject.hpp"
#include "Util/Renderer.hpp"
class FallingGround : public  Util::GameObject{
public:
    explicit  FallingGround() {

        m_Image->SetImage(RESOURCE_DIR"/Image/MapObject/platform.png");
        m_Drawable = m_Image;
    }
    void fall() {
        m_Transform.translation.y-=5;
    }
    void SetPosition(glm::vec2 position){m_Transform.translation = position;}
    static std::vector<std::shared_ptr<FallingGround>> CreateFromMap(
       const std::shared_ptr<MapInfoLoader>& mapLoader,
       Util::Renderer& rootRenderer) {
        std::vector<std::shared_ptr<FallingGround>> fallingGrounds;

        for (int y = 0; y < mapLoader->GetHeight() - 1; ++y) {
            for (int x = 0; x < mapLoader->GetWidth() - 1; ++x) {
                if (mapLoader->GetTile(x, y) == 7 &&
                    mapLoader->GetTile(x + 1, y) == 7 &&
                    mapLoader->GetTile(x + 2 , y) == 7 &&
                    mapLoader->GetTile(x , y + 1) == 7 &&
                    mapLoader->GetTile(x + 1, y + 1) == 7 &&
                    mapLoader->GetTile(x + 2, y + 1) == 7) {

                    float worldX = (x + 1) * 16 - 640;
                    float worldY = 480 - (y + 1) * 16;

                    auto fallingGround = std::make_shared<FallingGround>();
                    fallingGround->SetPosition({worldX, worldY});
                    fallingGround->SetZIndex(-2);
                    rootRenderer.AddChild(fallingGround);
                    fallingGrounds.push_back(fallingGround);
                    }
            }
        }

        return fallingGrounds;

    }
private:
    std::shared_ptr<Util::Image> m_Image;
};

#endif // FALLINGGROUND_HPP