//
// Created by andyl on 2025/3/8.
//

#ifndef RESOURCEMANAGER_HPP
#define RESOURCEMANAGER_HPP
#include "BackgroundImage.hpp"

class ResourceManager{
private:
    std::shared_ptr<BackgroundImage> m_Background;
public:
    ResourceManager();

    [[nodiscard]]  std::shared_ptr<Util::GameObject> GetChildren() const {
        return {m_Background};
    }
    void SetPhase(std::string phase);
};



#endif //RESOURCEMANAGER_HPP
