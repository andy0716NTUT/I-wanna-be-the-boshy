#pragma once
#include <array>
#include <memory>
#include <utility>
#include <cstdint>
namespace BoshyProfile {
enum class Kind { Image, Animation, Actor };
inline bool enabled = false;
inline std::array<std::uint64_t,3> created{};
template<Kind kind, class T, class... Args>
std::shared_ptr<T> Make(Args&&... args) {
    if (enabled) ++created[static_cast<unsigned>(kind)];
    return std::make_shared<T>(std::forward<Args>(args)...);
}
}
