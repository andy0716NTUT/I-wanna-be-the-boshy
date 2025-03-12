//
// Created by andyl on 2025/3/10.
//

#ifndef BULLET_HPP
#define BULLET_HPP
#include "Util/GameObject.hpp"


class Bullet : public Util::GameObject {
public:
    [[nodiscard]] const std::string& GetImagePath() const { return m_ImagePath; }

    [[nodiscard]] const glm::vec2& GetPosition() const { return m_Transform.translation; }

    [[nodiscard]] bool GetVisibility() const { return m_Visible; }

    void SetImage(const std::string& ImagePath);

    void SetPosition(const glm::vec2& Position) { m_Transform.translation = Position; }

    void Update(){}
private:
    inline std::string m_ImagePath = RESOURCE_DIR"/Image/Bullet";
};
#endif //BULLET_HPP
