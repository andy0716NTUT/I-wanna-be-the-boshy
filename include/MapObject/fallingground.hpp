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
    FallingGround(const std::string& imagePath) {
        SetImage(imagePath);
    }
    void SetFalling(bool falling) {
        this->isFalling = falling;
    }
    bool GetFalling(){return this->isFalling;}

    void SetPosition(glm::vec2 position){m_Transform.translation = position;}

    glm::vec2 GetPosition(){return m_Transform.translation;}

    void SetImage(const std::string& ImagePath) {
        m_ImagePath = ImagePath;

        m_Drawable = std::make_shared<Util::Image>(m_ImagePath);
    }
    static std::vector<std::shared_ptr<FallingGround>> CreateFromMap(
    const std::shared_ptr<MapInfoLoader>& mapLoader,
    Util::Renderer& rootRenderer) {

        std::vector<std::shared_ptr<FallingGround>> fallingGrounds;

        const int tileW = 6;
        const int tileH = 4;

        for (int y = 0; y <= mapLoader->GetHeight() - tileH; ++y) {
            for (int x = 0; x <= mapLoader->GetWidth() - tileW; ++x) {

                bool match = true;
                for (int dy = 0; dy < tileH && match; ++dy) {
                    for (int dx = 0; dx < tileW; ++dx) {
                        if (mapLoader->GetTile(x + dx, y + dy) != 7) {
                            match = false;
                            break;
                        }
                    }
                }
                if (match) {
                    float worldX = (x + tileW / 2.0f) * 16 - 640;
                    float worldY = 480 - (y + tileH / 2.0f) * 16;

                    auto fallingGround = std::make_shared<FallingGround>(RESOURCE_DIR "/Image/MapObject/platform.png");
                    fallingGround->SetPosition({worldX, worldY});
                    fallingGround->SetZIndex(-2);
                    rootRenderer.AddChild(fallingGround);
                    fallingGrounds.push_back(fallingGround);

                    x += tileW - 1; // 避免重疊偵測
                }
            }
        }

        return fallingGrounds;
    }

private:
    std::shared_ptr<Util::Image> m_Image;
    std::string m_ImagePath;
    bool m_Waiting = false; // 是否正在等待
    float m_WaitTimer = 0.0f; // 等待時間
    bool isFalling = false;
};

#endif // FALLINGGROUND_HPP