#ifndef MENU_HPP
#define MENU_HPP

#include "Util/GameObject.hpp"
#include "Util/Animation.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"

class Menu : public Util::GameObject {
public:
    enum class Option {
        START_GAME,
        SETTINGS,
        EXIT
    };

    Menu();
    void Update() ;  // 重寫 Update 方法處理輸入和動畫
    Option GetSelectedOption() const { return m_SelectedOption; }

private:
    std::shared_ptr<Util::Animation> m_Animation;
    int m_CurrentIndex;
    Option m_SelectedOption;


    void HandleInput();  // 處理按鍵輸入
};

#endif