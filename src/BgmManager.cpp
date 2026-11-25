//
// Created by andyl on 2025/5/1.
//

#include "BgmManager.hpp"
#include "Util/Logger.hpp"
#include <iostream>
#include <filesystem>
#include <chrono>
#include <stdexcept>

BgmManager::BgmManager(std::string SoundPath){
    if (!ValidateTrack(SoundPath)) throw std::runtime_error(m_LastError);
    const auto started = std::chrono::steady_clock::now();
    this->phase = SoundPath;
    m_TrackPath = SoundPath;
    m_BGM = std::make_shared<Util::BGM>(SoundPath);
    m_BGM->SetVolume(5);

    m_BGM->Play();
    m_BGM->FadeIn(2);
    m_LoadMilliseconds = std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-started).count();
    CheckPlayback();
}
void BgmManager::Fade() {
    m_BGM->FadeIn(2);
}
void BgmManager::SetBGM(std::string phase) {
    const auto path = BgmPath(phase);
    if (!ValidateTrack(path)) return; // Keep playing current music on invalid destination.
    const auto started = std::chrono::steady_clock::now();
    m_BGM->Pause();
    this->phase = phase;
    std::cout << "Load BGM : " << phase << std::endl;
    m_BGM->LoadMedia(path);
    m_TrackPath = path;
    m_BGM->FadeIn(2);
    m_BGM->Play();
    m_LoadMilliseconds = std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-started).count();
    CheckPlayback();
}

bool BgmManager::ValidateTrack(const std::string& path) {
    std::error_code error;
    if (!std::filesystem::is_regular_file(std::filesystem::u8path(path),error)) {
        m_LastError = "Missing/unreadable BGM: "+path+(error ? " ("+error.message()+")" : "");
        LOG_ERROR("{}",m_LastError);
        return false;
    }
    m_LastError.clear();
    return true;
}
void BgmManager::CheckPlayback() {
    if (!Mix_PlayingMusic()) {
        m_LastError = "Cannot play BGM: "+m_TrackPath+" ("+Mix_GetError()+")";
        LOG_ERROR("{}",m_LastError);
    }
}
