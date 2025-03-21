
#include "ResourceManager.hpp"
ResourceManager::ResourceManager() {
    m_Background = std::make_shared<BackgroundImage>();
}

void ResourceManager::NextPhase() {
    Background++;
    m_Background->NextPhase(Background);
}
