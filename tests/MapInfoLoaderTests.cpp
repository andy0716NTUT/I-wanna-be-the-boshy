#include "MapInfoLoader.hpp"

#include <filesystem>
#include <stdexcept>

namespace {
void Check(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}
// The original LoadMap loop, retained as an independent oracle for valid assets.
std::vector<std::vector<int>> OriginalMap(const std::filesystem::path& path) {
    std::ifstream file(path);
    Check(bool(file), "Cannot open original map oracle");
    std::vector<std::vector<int>> result;
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::vector<int> row;
        int value;
        while (ss >> value) {
            row.push_back(value);
            if (ss.peek() == ',') ss.ignore();
        }
        if (!row.empty()) result.push_back(row);
    }
    return result;
}

std::vector<std::vector<int>> OriginalTileData(const std::filesystem::path& path) {
    std::ifstream file(path);
    Check(bool(file), "Cannot open original tile-data oracle");
    std::vector<std::vector<int>> result;
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::vector<int> row;
        std::string token;
        while (std::getline(ss, token, ',')) {
            try { row.push_back(std::stoi(token)); }
            catch (...) { row.push_back(0); }
        }
        result.push_back(row);
    }
    return result;
}
}

int main(int argc, char** argv) try {
    Check(argc == 2, "Expected fixture directory");
    const auto fixture = std::filesystem::absolute(argv[1]);
    const auto maps = fixture / "Image/Background/TEST" / std::filesystem::u8path(u8"測試");
    std::filesystem::create_directories(maps);
    const auto write = [&](const char* name, const char* contents) {
        const auto path = maps / name;
        std::ofstream(path) << contents;
        return path.u8string();
    };
    MapInfoLoader loader;
    std::size_t count = 0;
    for (const auto& world : std::filesystem::directory_iterator(
             std::filesystem::path(RESOURCE_DIR) / "Image/Background")) {
        if (!world.is_directory()) continue;
        for (const auto& entry : std::filesystem::directory_iterator(world)) {
            if (entry.path().extension() != ".txt") continue;
            const auto phase = entry.path().stem().string().substr(3);
            loader.LoadMap(phase, world.path().filename().string());
            const auto expected = OriginalMap(entry.path());
            Check(loader.GetMapData() == expected, "Valid asset changed vs original parser");
            Check(loader.GetWidth() == 80 && loader.GetHeight() == 60,
                  "Unexpected original map dimensions");
            Check(loader.GetTile(-1, 0) == -1 && loader.GetTile(80, 0) == -1 &&
                  loader.GetTile(0, -1) == -1 && loader.GetTile(0, 60) == -1,
                  "Out-of-range read did not return sentinel");
            const int before = loader.GetTile(0, 0);
            loader.SetTile(0, 0, 69);
            Check(loader.GetTile(0, 0) == 69, "SetTile failed");
            loader.SetTile(-1, 0, 5);
            loader.SetTile(80, 60, 5);
            loader.LoadMap(phase, world.path().filename().string());
            Check(loader.GetTile(0, 0) == before, "Reload retained modified tile");
            ++count;
        }
    }
    Check(count > 0, "No existing maps tested");
    std::size_t masks = 0;
    for (const auto* directory : {"Image/enemy", "Image/MapObject"}) {
        for (const auto& entry : std::filesystem::directory_iterator(
                 std::filesystem::path(RESOURCE_DIR) / directory)) {
            if (entry.path().extension() != ".txt") continue;
            Check(MapInfoLoader::LoadTileData(entry.path().u8string()) ==
                  OriginalTileData(entry.path()), "Original trap/spider mask data changed");
            ++masks;
        }
    }
    Check(masks > 0, "No original trap masks tested");
    Check(MapInfoLoader::LoadTileData(write("valid.txt", "1,2,3\n\n4 5 6\n")) ==
          std::vector<std::vector<int>>{{1,2,3},{4,5,6}}, "CSV/blank row compatibility");
    for (const auto& bad : {
             write("ragged.txt", "1,2\n3\n"),
             write("invalid.txt", "1,wat\n2,3\n"),
             write("overflow.txt", "1,999999999999999999999999\n"),
             write("empty.txt", "\n  \n"),
             (maps / "missing.txt").u8string()}) {
        Check(MapInfoLoader::LoadTileData(bad).empty(), "Malformed map accepted");
    }
    loader.LoadMap("does-not-exist", "WORLD1");
    Check(loader.GetWidth() == 0 && loader.GetHeight() == 0 &&
          loader.GetMapData().empty() && loader.GetTile(0,0) == -1,
          "Missing phase retained stale map");
    loader.SetTile(0,0,5);
    std::cout << count << " original maps and " << masks
              << " trap masks matched cell-for-cell; malformed/bounds/reload passed\n";
    return 0;
} catch (const std::exception& error) {
    std::cerr << error.what() << '\n';
    return 1;
}
