#include "App.hpp"


App::App()
    : m_BGM(RESOURCE_DIR "/BGM/FirstWorld.mp3"){}

void App::End() { // NOLINT(this method will mutate members in the future)
    LOG_TRACE("End");
}
