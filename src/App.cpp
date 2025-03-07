#include "App.hpp"
#include "ParserBackground.hpp"
#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include <windows.h>
void App::Start() {
    LOG_TRACE("Start");
    SetConsoleOutputCP(65001);
    ParserBackground parser("C:/OOPL/I-wanna-cry/Resources/Map/map0-1.tmx");

    if (parser.load()) {
        parser.printLayerData();
    }
    m_CurrentState = State::UPDATE;

}

void App::Update() {

    //TODO: do your things here and delete this line <3

    /*
     * Do not touch the code below as they serve the purpose for
     * closing the window.
     */
    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) ||
        Util::Input::IfExit()) {
        m_CurrentState = State::END;
        }
}

void App::End() { // NOLINT(this method will mutate members in the future)
    LOG_TRACE("End");
}