
#include "ResourceManager.hpp"
ResourceManager::ResourceManager() {
    m_Background = std::make_shared<BackgroundImage>();
}

void ResourceManager::SetPhase(std::string phase) {
    m_Background->SetPhase(phase);
}
