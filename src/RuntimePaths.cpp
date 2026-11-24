#include "RuntimePaths.hpp"

#include <stdexcept>

void UseRuntimeDirectory(const std::filesystem::path& directory) {
    const auto base = std::filesystem::absolute(directory);
    for (const auto* relative : {"Resources", "PTSDAssets/shaders/Base.vert",
                                 "PTSDAssets/shaders/Base.frag", "config.json"}) {
        const auto path = base / relative;
        const bool valid = std::string(relative) == "Resources"
            ? std::filesystem::is_directory(path)
            : std::filesystem::is_regular_file(path);
        if (!valid) {
            throw std::runtime_error("Missing runtime resource: " + path.u8string());
        }
    }
    std::filesystem::current_path(base);
}
