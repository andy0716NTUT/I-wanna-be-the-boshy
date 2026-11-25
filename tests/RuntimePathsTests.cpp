#include "RuntimePaths.hpp"

#include <fstream>
#include <iostream>
#include <stdexcept>

int main(int argc, char** argv) try {
    if (argc != 2) throw std::runtime_error("Expected fixture directory");
    const auto original = std::filesystem::current_path();
    const auto fixture = std::filesystem::absolute(argv[1]);
    std::filesystem::create_directories(fixture / "Resources");
    std::filesystem::create_directories(fixture / "PTSDAssets/shaders");
    for (const auto* file : {"PTSDAssets/shaders/Base.vert", "PTSDAssets/shaders/Base.frag",
                             "config.json"}) {
        std::ofstream(fixture / file) << (std::string(file)=="config.json" ? "{}" : "fixture");
    }
    UseRuntimeDirectory(fixture);
    if (std::filesystem::current_path() != fixture || !std::ifstream("config.json")) {
        throw std::runtime_error("Did not resolve configuration from executable directory");
    }
    std::filesystem::current_path(original);
    std::filesystem::remove(fixture / "PTSDAssets/shaders/Base.frag");
    try {
        UseRuntimeDirectory(fixture);
    } catch (const std::runtime_error& error) {
        if (std::string(error.what()).find("Base.frag") == std::string::npos) throw;
        if (std::filesystem::current_path() != original) {
            throw std::runtime_error("Failed validation changed the process directory");
        }
        std::cout << "Runtime path selection and missing-resource diagnostics passed\n";
        return 0;
    }
    throw std::runtime_error("Missing shader was accepted");
} catch (const std::exception& error) {
    std::cerr << error.what() << '\n';
    return 1;
}
