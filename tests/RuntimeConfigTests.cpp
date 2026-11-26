#include "RuntimeConfig.hpp"
#include <iostream>
int main(int argc,char** argv) try {
    if (argc!=2) throw std::runtime_error("Expected fixture path");
    const auto path=std::filesystem::absolute(argv[1]);
    std::filesystem::create_directories(path.parent_path());
    for (const auto* text : {"{}", "{\"fullscreen\":true,\"window_width\":3840,\"window_height\":2160,\"fps_cap\":0}"}) {
        std::ofstream(path)<<text;
        if (ReadRuntimeOptions(path).weightedPlayer) throw std::runtime_error("Default feel changed");
    }
    for (const auto* feel : {"original", "weighted"}) {
        std::ofstream(path)<<"{\"player_feel\":\""<<feel<<"\"}";
        if (ReadRuntimeOptions(path).weightedPlayer != (std::string(feel)=="weighted"))
            throw std::runtime_error("Wrong configured feel");
    }
    for (const auto* text : {"{", "[]", "{\"window_width\":0}", "{\"window_height\":-1}",
        "{\"window_width\":9999999999}", "{\"fps_cap\":-1}", "{\"fps_cap\":1.5}",
        "{\"fullscreen\":1}", "{\"title\":false}", "{\"default_log_level\":7}",
        "{\"player_feel\":false}", "{\"player_feel\":\"unknown\"}", "{\"player_feel\":null}"}) {
        std::ofstream(path)<<text;
        bool rejected=false;
        try { ReadRuntimeOptions(path); } catch (const std::runtime_error& error) {
            rejected=std::string(error.what()).find(path.u8string())!=std::string::npos;
        }
        if (!rejected) throw std::runtime_error("Invalid configuration accepted without path");
    }
    return 0;
} catch(const std::exception& e) {std::cerr<<e.what()<<'\n';return 1;}
