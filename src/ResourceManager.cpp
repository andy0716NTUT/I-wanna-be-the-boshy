
#include "ResourceManager.hpp"
ResourceManager::ResourceManager() {
    m_Background = std::make_shared<BackgroundImage>();
}

void ResourceManager::SetPhase(std::string phase,std::string CurrentWorld) {
    m_Background->SetPhase(phase, CurrentWorld);
}
void ResourceManager::rotate(float deltaTime) {
    m_Background->rotate(deltaTime);
}
void ResourceManager::resetRotation() {
    m_Background->resetRotation();
}

