#include "Bullet.hpp"

void Bullet::SetImage(const std::string& imagePath) {
    ImagePath = imagePath;
    std::vector<std::string> imagePaths = {imagePath};
    m_Bullet = std::make_shared<Util::Animation>(imagePaths, true, 100, true);
    SetDrawable(m_Bullet);
}

void Bullet::SetPosition(const glm::vec2& position) {
    m_Transform.translation = position;
}

void Bullet::Update(float deltaTime) {
    if (lifeTime > 0) {
        lifeTime -= deltaTime;
        if (lifeTime <= 0) {
            SetVisible(false);
            SetDrawable(nullptr); // 清除圖片資源
            m_Bullet.reset();     // 釋放動畫資源
        }
    }
}

bool Bullet::UpdateWithCollision(float deltaTime, const std::shared_ptr<MapInfoLoader>& mapLoader) {
    Update(deltaTime);

    if (!IsVisible()) return false;

    // 移动子弹
    glm::vec2 bulletPosition = GetPosition();
    if (GetDirection() == Character::direction::LEFT) {
        bulletPosition.x -= BULLET_SPEED;
    } else {
        bulletPosition.x += BULLET_SPEED;
    }
    SetPosition(bulletPosition);

    // 检查地图碰撞
    int bulletTileX = static_cast<int>((bulletPosition.x + 640) / 16);
    int bulletTileY = static_cast<int>((480 - bulletPosition.y) / 16);

    if (bulletTileX >= 0 && bulletTileX < mapLoader->GetWidth() &&
        bulletTileY >= 0 && bulletTileY < mapLoader->GetHeight()) {

        int tileValue = mapLoader->GetTile(bulletTileX, bulletTileY);
        if (tileValue == 1 || tileValue == 2 || tileValue == 5) {
            SetVisible(false);
            SetDrawable(nullptr); // 清除图片资源
            return true; // 碰撞发生
        }
    }

    return false; // 无碰撞
}

bool Bullet::CheckCheckpointCollision(const std::vector<std::shared_ptr<CheckPoint>>& checkpoints,
                                    glm::vec2& outCheckpointPos, std::string& outPhase,
                                    int& outX, int& outY) {
    if (!IsVisible()) return false;

    for (auto& checkpoint : checkpoints) {
        glm::vec2 cpPos = checkpoint->GetPosition();
        if (glm::distance(cpPos, GetPosition()) < 20.0f) {
            checkpoint->play();
            return true; // 检测到碰撞
        }
    }

    return false; // 无碰撞
}

std::shared_ptr<Bullet> Bullet::CreateBullet(
    const glm::vec2& position,
    Character::direction direction,
    float lifeTime,
    Util::Renderer& renderer) {

    auto bullet = std::make_shared<Bullet>();
    bullet->SetPosition(position);
    bullet->SetLifeTime(lifeTime);
    bullet->SetVisible(true);
    bullet->SetDirection(direction);

    if (direction == Character::direction::LEFT) {
        bullet->SetImage(RESOURCE_DIR"/Image/bullet2.png");
    } else {
        bullet->SetImage(RESOURCE_DIR"/Image/bullet.png");
    }

    renderer.AddChild(bullet);
    return bullet;
}

void Bullet::CleanBullet(std::vector<std::shared_ptr<Bullet>>& bullets) {
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
        [](const std::shared_ptr<Bullet>& bullet) {
            return !bullet->IsVisible();
        }), bullets.end());
}