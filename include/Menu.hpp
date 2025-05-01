#ifndef MENU_HPP
#define MENU_HPP

#include "Util/GameObject.hpp"
#include "Util/Animation.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"

class Menu : public Util::GameObject{
public:
    std::vector<int > Save  = {1,2,3};
    enum class Option {
        START_GAME,
        SAVE,
        SETTINGS,
    };

    Menu();

    bool StartGame();

    void SetImage(const std::string& ImagePath) {
        m_ImagePath = ImagePath;
        m_Drawable = std::make_shared<Util::Image>(m_ImagePath);
    }

    Option GetSelectedOption() const { return m_SelectedOption; }

    void Update(float deltaTime);

    void UpdateSaveIndex() {
        this->SetImage(RESOURCE_DIR"/Image/Menu/menu_save" + std::to_string(savetIndex+1) +".png");
    }

private:
    std::string m_ImagePath;
    int savetIndex = 0;
    float coolTimer = 0.0f;
    bool isStart = false;
    Option m_SelectedOption = Option::START_GAME;



};

#endif