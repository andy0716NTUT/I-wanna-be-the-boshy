//
// Created by andyl on 2025/5/7.
//

#ifndef BOSSINFOTEXT_HPP
#define BOSSINFOTEXT_HPP

#include "Util/GameObject.hpp"
#include "Util/Text.hpp"
class BossInfoText : public  Util::GameObject{
public:
    BossInfoText();
    void SetText();
    void SetBossHealth(int damage);
    int GetBossHealth();
private:
    std::shared_ptr<Util::Text> m_Text;
    int BossHealth = 100;
};
#endif //BOSSINFOTEXT_HPP
