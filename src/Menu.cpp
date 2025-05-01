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

void Menu::Update() {
    if (Util::Input::IsKeyPressed(Util::Keycode::RETURN)) {
        if (m_SelectedOption == Option::START_GAME) {
            m_SelectedOption = Option::SAVE;
            UpdateSaveIndex();
        }
    }else if (Util::Input::IsKeyDown(Util::Keycode::DOWN)) {
        if (m_SelectedOption == Option::START_GAME) {
            m_SelectedOption = Option::SETTINGS;
            this->SetImage(RESOURCE_DIR"/Image/Menu/menu_option.png");
        }
    }else if (Util::Input::IsKeyDown(Util::Keycode::UP) && m_SelectedOption == Option::SETTINGS) {
        m_SelectedOption =  Option::START_GAME;
        SetImage(RESOURCE_DIR"/Image/Menu/menu_game.png");
    }

    if (m_SelectedOption == Option::SAVE) {
        if (Util::Input::IsKeyDown(Util::Keycode::RIGHT)) {
            savetIndex += 1;
        }else if (Util::Input::IsKeyDown(Util::Keycode::LEFT)) {
            savetIndex -= 1;
        }else if (Util::Input::IsKeyPressed(Util::Keycode::RETURN)) {
            this->isStart = true;
        }

        if (savetIndex > 2)savetIndex = 2;
        if (savetIndex < 0)savetIndex = 0;

        UpdateSaveIndex();
    }
}
