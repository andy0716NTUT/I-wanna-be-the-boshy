//
// Created by andyl on 2025/6/12.
//
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#ifndef GAMEOVERUI_HPP
#define GAMEOVERUI_HPP
class GameOverUI : public Util::GameObject {
public:
    GameOverUI();
    void SetPosition(const glm::vec2& position);
    void Show();
    void Hide();
    void Update(float deltaTime);
private:
    std::shared_ptr<Util::Image> m_GameOverText;
    std::shared_ptr<Util::Image> m_PressRText;
    float m_Timer = 0.0f;
};
#endif //GAMEOVERUI_HPP
