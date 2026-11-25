#define SDL_MAIN_HANDLED
#include "BgmManager.hpp"
#include "Util/Logger.hpp"
#include <stdexcept>
int main() {
    SDL_setenv("SDL_AUDIODRIVER","dummy",1);
    Util::Logger::Init();
    if (SDL_Init(SDL_INIT_AUDIO)!=0 || Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,2048)!=0) return 1;
    if (std::string(SDL_GetCurrentAudioDriver())!="dummy") return 2;
    {
        BgmManager music(RESOURCE_DIR "/BGM/MENU.mp3");
        for (const auto* track : {"WORLD1","WORLD2","Boss1","WORLD1","WORLD1"}) {
            music.SetBGM(track);
            if (!music.GetLastError().empty() || !Mix_PlayingMusic() || Mix_VolumeMusic(-1)!=5)
                throw std::runtime_error("Silent BGM load/restart/default-volume check failed");
        }
        const auto previous=music.GetTrackPath();
        music.SetBGM("does-not-exist");
        if (music.GetLastError().empty() || music.GetTrackPath()!=previous || !Mix_PlayingMusic()) return 3;
        Mix_HaltMusic();
    }
    Mix_CloseAudio(); SDL_Quit(); return 0;
}
