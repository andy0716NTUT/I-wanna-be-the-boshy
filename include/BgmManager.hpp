//
// Created by andyl on 2025/5/1.
//

#ifndef BGMMANAGER_HPP
#define BGMMANAGER_HPP
#include "Util/BGM.hpp"


class BgmManager {
    public:
        BgmManager();
        void SetBGM(std::string phase);
    private:
            Util::BGM m_BGM;
            inline  std::string BgmPath(std::string phase) {
                return RESOURCE_DIR"/BGM/" + phase + ".mp3";
            }
};



#endif //BGMMANAGER_HPP
