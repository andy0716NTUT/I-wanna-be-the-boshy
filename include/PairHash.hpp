#ifndef PAIRHASH_HPP
#define PAIRHASH_HPP

#include <utility>
#include <functional>

namespace Util {
    struct PairHash {
        template <typename T1, typename T2>
        std::size_t operator()(const std::pair<T1, T2>& pair) const {
            auto hash1 = std::hash<T1>{}(pair.first);
            auto hash2 = std::hash<T2>{}(pair.second);
            return hash1 ^ (hash2 << 1); // 簡單的哈希組合
        }
    };
}

#endif // PAIRHASH_HPP