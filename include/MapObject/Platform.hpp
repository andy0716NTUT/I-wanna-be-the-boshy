//
// Created by andyl on 2025/4/18.
//

#ifndef PLATFORM_HPP
#define PLATFORM_HPP
#include "Util/GameObject.hpp"
class Platform : public  Util::GameObject{
public:
    Platform(const std::string& imagePath) {
        SetImage(imagePath);
    }
    void SetDisappear(bool dis){this->isDisapear = dis ;}
    bool GetDIsappear(){return this->isDisapear;}

    void SetPosition(glm::vec2 position){m_Transform.translation = position;}
    glm::vec2 GetPosition(){return m_Transform.translation ;}

    void SetImage(const std::string& ImagePath) {
        m_ImagePath = ImagePath;

        m_Drawable = std::make_shared<Util::Image>(m_ImagePath);
    }

    static std::vector<std::shared_ptr<Platform>> CreateFromMap(
    const std::shared_ptr<MapInfoLoader>& mapLoader,
    Util::Renderer& rootRenderer) {

        std::vector<std::shared_ptr<Platform>> platforms;
        for (int y = 0; y <= mapLoader->GetHeight() - 1; ++y) {
            for (int x = 0; x <= mapLoader->GetWidth() - 1; ++x) {

                if (mapLoader->GetTile(x, y) == 8 &&
                     mapLoader->GetTile(x + 1, y) == 8 &&
                     mapLoader->GetTile(x, y + 1) == 8 &&
                     mapLoader->GetTile(x + 1, y + 1) == 8) {

                    float worldX = (x + 1) * 16 - 640;
                    float worldY = 480 - (y + 1) * 16;

                    auto platform = std::make_shared<Platform>(RESOURCE_DIR "/Image/MapObject/small_ground.png");
                    platform->SetPosition({worldX, worldY});
                    platform->SetZIndex(-2);
                    rootRenderer.AddChild(platform);
                    platforms.push_back(platform);

                    x += 1;

                     }
            }
        }

        return platforms;
    }
private:
    std::string m_ImagePath;
    bool isDisapear;
};
#endif //PLATFORM_HPP