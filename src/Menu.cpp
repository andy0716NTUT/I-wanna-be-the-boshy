#include "Menu.hpp"

#include <iostream>

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"

Menu::Menu() {
    m_SelectedOption = Option::START_GAME;
}

bool Menu::StartGame() {
    return this->isStart;
}

void Menu::Update(float deltaTime) {
    if (coolTimer > 0.0f) {
        coolTimer -= deltaTime;
        return; // 冷卻中 → 不處理輸入
    }

    if (m_SelectedOption == Option::START_GAME) {
        if (Util::Input::IsKeyPressed(Util::Keycode::RETURN)) {
            m_SelectedOption = Option::SAVE;
            UpdateSaveIndex();
            coolTimer = 0.2f; // 冷卻 0.2 秒
        }
        else if (Util::Input::IsKeyPressed(Util::Keycode::DOWN)) {
            m_SelectedOption = Option::SETTINGS;
            SetImage(RESOURCE_DIR"/Image/Menu/menu_option.png");
            coolTimer = 0.2f;
        }
    }
    else if (m_SelectedOption == Option::SETTINGS) {
        if (Util::Input::IsKeyPressed(Util::Keycode::UP)) {
            m_SelectedOption = Option::START_GAME;
            SetImage(RESOURCE_DIR"/Image/Menu/menu_game.png");
            coolTimer = 0.2f;
        }
    }
    else if (m_SelectedOption == Option::SAVE) {
        if (Util::Input::IsKeyPressed(Util::Keycode::RIGHT)) {
            savetIndex += 1;
            coolTimer = 0.2f;
        }
        else if (Util::Input::IsKeyPressed(Util::Keycode::LEFT)) {
            savetIndex -= 1;
            coolTimer = 0.2f;
        }
        else if (Util::Input::IsKeyPressed(Util::Keycode::RETURN)) {
            this->isStart = true;
            coolTimer = 0.2f;
        }

        if (savetIndex > 2) savetIndex = 2;
        if (savetIndex < 0) savetIndex = 0;

        UpdateSaveIndex();
    }
}
