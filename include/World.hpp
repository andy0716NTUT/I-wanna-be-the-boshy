//
// Created by andyl on 2025/4/4.
//

#ifndef WORLD_HPP
#define WORLD_HPP
#include <iostream>
#include <vector>
class World {
    public:
            const std::vector<std::vector<std::string>> world1 = {
                { "1",  "2",  "3",  "4",  "5","-" },
                { "6",  "-",  "-",  "-",  "-","-" },
                 { "7",  "8",  "9",  "10",  "11", "12","13","14"}};

            enum class Direction { NONE, UP, DOWN, LEFT, RIGHT };

            inline const std::vector<std::vector<std::string>>& GetWorldByPhaseName(const std::string& phaseName) {
                if (phaseName == "WORLD1") return world1;
                return world1; // 預設值
            }
            Direction GetTeleportDirection(int tile) {
                switch (tile) {
                    case 69: return Direction::RIGHT;
                    case 68: return Direction::LEFT;
                    case 67: return Direction::UP;
                    case 66: return Direction::DOWN;
                    default: return Direction::NONE;
                }
            }
            inline std::pair<int, int> GetStartPosition(const std::vector<std::vector<std::string>>& world, std::string phasetId) {
                    for (int y = 0; y < static_cast<int>(world.size()); ++y) {
                        for (int x = 0; x < static_cast<int>(world[y].size()); ++x) {
                            if (world[y][x] == phasetId) {
                                return {x, y};
                            }
                        }
                    }
                    return {-1, -1}; // 找不到起點
                }
    inline std::pair<int, int> GetPhaseIndex(const std::string& phaseId, const std::string& worldName) {
                const auto& world = GetWorldByPhaseName(worldName);

                // 取 phaseId 的第一段數字（例如 "3_1" => "3"）
                std::string basePhase = phaseId.substr(0, phaseId.find('_'));

                for (int y = 0; y < static_cast<int>(world.size()); ++y) {
                    for (int x = 0; x < static_cast<int>(world[y].size()); ++x) {
                        if (world[y][x] == basePhase) {
                            return {x, y};
                        }
                    }
                }

                return {-1, -1}; // 沒找到
            }
};



#endif //WORLD_HPP
