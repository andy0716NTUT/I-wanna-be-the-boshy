//
// Created by andyl on 2025/5/7.
//

#ifndef BOSSHPINFO_HPP
#define BOSSHPINFO_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"

class BossHpInfo : public Util::GameObject {
private:
    std::shared_ptr<Util::Image> m_Image;
public:
    BossHpInfo() {
        m_Image = std::make_shared<Util::Image>(RESOURCE_DIR"/Image/Boss1/BossHp.png");
        SetDrawable(m_Image);
        m_Transform.translation = {0,464};
    }

    void SetScaleX(float scaleX) {
        m_Transform.scale.x = scaleX;

        float halfWidth = 1280.0 / 2.0f;  // 需要在初始化時存原始寬度
        m_Transform.translation.x = 0 - (1.0f - scaleX) * halfWidth;
    }
};
#endif //BOSSHPINFO_HPP
