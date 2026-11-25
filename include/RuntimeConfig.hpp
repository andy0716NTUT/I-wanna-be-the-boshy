#pragma once
#include <filesystem>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <nlohmann/json.hpp>
struct RuntimeOptions { bool fullscreen = false; };
inline RuntimeOptions ReadRuntimeOptions(const std::filesystem::path& path) {
    try {
        std::ifstream file(path);
        if (!file) throw std::runtime_error("cannot open file");
        const auto json = nlohmann::json::parse(file);
        if (!json.is_object()) throw std::runtime_error("root must be an object");
        const auto integer = [&](const char* key, double low, double high) {
            if (!json.contains(key)) return;
            const auto& value=json.at(key);
            if (!value.is_number_integer() || value.get<double>()<low || value.get<double>()>high)
                throw std::runtime_error(std::string(key)+" must be an integer in ["+std::to_string(low)+", "+std::to_string(high)+"]");
        };
        integer("window_width",320,8192); integer("window_height",240,8192);
        integer("fps_cap",0,std::numeric_limits<unsigned>::max());
        integer("default_log_level",0,6);
        if (json.contains("title") && !json.at("title").is_string()) throw std::runtime_error("title must be a string");
        if (json.contains("fullscreen") && !json.at("fullscreen").is_boolean()) throw std::runtime_error("fullscreen must be boolean");
        return {json.value("fullscreen",false)};
    } catch (const std::exception& error) {
        throw std::runtime_error("Invalid configuration "+path.u8string()+": "+error.what());
    }
}
