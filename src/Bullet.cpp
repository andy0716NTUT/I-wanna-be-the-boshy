//
// Created by howar on 2025/3/21.
//
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

void Bullet::CleanBullet(std::vector<std::shared_ptr<Bullet>>& bullets) {
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
        [](const std::shared_ptr<Bullet>& bullet) {
            return !bullet->IsVisible();
        }), bullets.end());
}