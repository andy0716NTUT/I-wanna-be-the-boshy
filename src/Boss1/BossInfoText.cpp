//
// Created by andyl on 2025/5/7.
//

#include "Boss1/BossInfoText.hpp"

BossInfoText::BossInfoText() {
    m_Text = std::make_shared<Util::Text>(RESOURCE_DIR"/Font/GlowSansSC.otf",20,"Boss Health : " + std::to_string(BossHealth));
    this->SetDrawable(m_Text);
    m_Transform.translation = {0,-400};
}
void BossInfoText::SetText() {
    m_Text->SetText("Boss Health : " + std::to_string(BossHealth));

}
void BossInfoText::SetBossHealth(int damage) {
    this->BossHealth -= damage;
}

int BossInfoText::GetBossHealth() {
    return this->BossHealth;
}



