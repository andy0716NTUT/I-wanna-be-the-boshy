#ifndef ANIMATED_CHARACTER_HPP
#define ANIMATED_CHARACTER_HPP

#include <vector>
#include <string>
#include <unordered_map>

#include "Util/Animation.hpp"
#include "Util/GameObject.hpp"
#include "Character.hpp"

class AnimatedCharacter : public Character {
public:
    explicit AnimatedCharacter(const std::unordered_map<MoveState, std::vector<std::string>>& animationPaths);

    void SetState(MoveState state); // 切換狀態並播放對應動畫

    [[nodiscard]] bool IsLooping() const {
        return std::dynamic_pointer_cast<Util::Animation>(m_Drawable)->GetLooping();
    }

    [[nodiscard]] bool IsPlaying() const {
        return std::dynamic_pointer_cast<Util::Animation>(m_Drawable)->GetState() == Util::Animation::State::PLAY;
    }

    void SetLooping(bool looping) {
        std::dynamic_pointer_cast<Util::Animation>(m_Drawable)->SetLooping(looping);
    }

    [[nodiscard]] bool IfAnimationEnds() const {
        auto anim = std::dynamic_pointer_cast<Util::Animation>(m_Drawable);
        return anim && anim->GetState() == Util::Animation::State::ENDED;
    }

private:
    std::unordered_map<MoveState, std::shared_ptr<Util::Animation>> m_Animations; // 儲存每個狀態的動畫
};

#endif //ANIMATED_CHARACTER_HPP