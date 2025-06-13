//
// Created by andyl on 2025/5/1.
//

#include "BgmManager.hpp"
#include <iostream>

BgmManager::BgmManager(std::string SoundPath){
    this->phase = SoundPath;
    m_BGM = std::make_shared<Util::BGM>(SoundPath);
    m_BGM->SetVolume(5);

    m_BGM->Play();
    m_BGM->FadeIn(2);

}
void BgmManager::Fade() {
    m_BGM->FadeIn(2);
}
void BgmManager::SetBGM(std::string phase) {
    m_BGM->Pause();
    this->phase = phase;
    std::cout << "Load BGM : " << phase << std::endl;
    m_BGM->LoadMedia(BgmPath(phase));
    m_BGM->FadeIn(2);
    m_BGM->Play();
}