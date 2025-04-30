#include "AnimatedCharacter.hpp"

AnimatedCharacter::AnimatedCharacter(const std::unordered_map<MoveState, std::vector<std::string>>& animationPaths)
    : Character(animationPaths.at(MoveState::IDLE)[0]) { // 初始使用 idle 的第一幀
    // 載入所有動畫
    for (const auto& [state, paths] : animationPaths) {
        m_Animations[state] = std::make_shared<Util::Animation>(paths, true, 450);
    }
    SetState(MoveState::IDLE); // 初始狀態為 idle
}

void AnimatedCharacter::SetState(MoveState state) {
    if (GetState() != state) { // 避免重複切換相同狀態
        Character::SetState(state);
        auto animation = m_Animations[state];
        m_Drawable = animation; // 切換當前動畫
        animation->SetLooping(state != MoveState::SHOOT); // shoot 不循環，其他狀態循環
        animation->Play(); // 開始播放
    }
}
