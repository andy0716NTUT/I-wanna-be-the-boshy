#include "World.hpp"
#include <stdexcept>

int main() {
    World world;
    const auto check = [](bool ok) { if (!ok) throw std::runtime_error("World lookup mismatch"); };
    for (int row = 0; row < static_cast<int>(world.world1.size()); ++row) {
        for (int column = 0; column < static_cast<int>(world.world1[row].size()); ++column) {
            const auto& phase = world.world1[row][column];
            check(world.GetPhaseAt("WORLD1", row, column) == phase);
            if (phase != "-") check(world.GetPhaseIndex(phase, "WORLD1") == std::make_pair(column, row));
        }
    }
    check(world.GetPhaseIndex("3_2", "WORLD1") == std::make_pair(2, 0));
    check(world.GetPhaseIndex("unknown", "WORLD1") == std::make_pair(-1, -1));
    check(world.GetPhaseAt("WORLD2", 0, 0) == "1"); // Retain original shared layout fallback.
    for (const auto index : {std::pair{-1,0}, {0,-1}, {3,0}, {0,6}, {1,6}}) {
        bool rejected = false;
        try { world.GetPhaseAt("WORLD1", index.first, index.second); }
        catch (const std::out_of_range& error) {
            rejected = std::string(error.what()).find("WORLD1") != std::string::npos;
        }
        check(rejected);
    }
    check(world.GetTeleportDirection(69) == World::Direction::RIGHT);
    check(world.GetTeleportDirection(68) == World::Direction::LEFT);
    check(world.GetTeleportDirection(67) == World::Direction::UP);
    check(world.GetTeleportDirection(66) == World::Direction::DOWN);
    check(world.GetTeleportDirection(5) == World::Direction::NONE);
}
