#include "GlDebugLifetime.hpp"
#include "FrameCapture.hpp"
#include "RegressionAccess.hpp"
#include "Core/Context.hpp"
#include "RuntimePaths.hpp"

#include <cstdlib>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <string>

namespace {
void Key(SDL_Scancode key, bool down) {
    SDL_Event event{};
    event.type = down ? SDL_KEYDOWN : SDL_KEYUP;
    event.key.state = down ? SDL_PRESSED : SDL_RELEASED;
    event.key.keysym.scancode = key;
    event.key.keysym.sym = SDL_GetKeyFromScancode(key);
    if (SDL_PushEvent(&event) != 1) throw std::runtime_error(SDL_GetError());
}
glm::vec2 TileCenter(int x, int y) { return {x * 16.0f - 632, 472 - y * 16.0f}; }
glm::vec2 FindFloor(App& app, int id, int headroom) {
    const auto map = BoshyRegressionAccess::Map(app);
    for (int y = headroom + 1; y < map->GetHeight() - 2; ++y) {
        for (int x = 3; x < map->GetWidth() - 3; ++x) {
            if (map->GetTile(x, y) != id) continue;
            bool clear = true;
            for (int above = 1; above <= headroom; ++above)
                for (int dx = -1; dx <= 1; ++dx)
                    clear = clear && map->GetTile(x + dx, y - above) == 0;
            if (clear) return {x * 16.0f - 632, 480 - (y - 1) * 16.0f - 4};
        }
    }
    throw std::runtime_error("Existing map has no requested floor fixture");
}
glm::vec2 FindTile(App& app, int id) {
    const auto map = BoshyRegressionAccess::Map(app);
    for (int y = 2; y < map->GetHeight() - 2; ++y)
        for (int x = 2; x < map->GetWidth() - 2; ++x)
            if (map->GetTile(x,y) == id) return TileCenter(x,y);
    throw std::runtime_error("Existing map has no requested tile fixture");
}
glm::vec2 FindContact(App& app, bool head) {
    const auto map = BoshyRegressionAccess::Map(app);
    for (int y = 2; y < map->GetHeight() - 2; ++y)
        for (int x = 2; x < map->GetWidth() - 2; ++x)
            if (map->GetTile(x,y) == 0 && map->GetTile(x + (head ? 0 : 1), y - (head ? 1 : 0)) == 2 &&
                map->GetTile(x,y+1) != 5 && map->GetTile(x-1,y) != 5)
                return TileCenter(x,y);
    throw std::runtime_error("Existing map has no requested solid-contact fixture");
}
}

int main(int argc, char** argv) try {
    if (argc < 3 || argc > 4) throw std::runtime_error("Usage: probe TRACE_PATH SCENARIO [hold-after-frames]");
    const auto output = std::filesystem::absolute(argv[1]);
    const std::string scenario = argv[2];
    SDL_setenv("SDL_AUDIODRIVER", "dummy", 1);
    const std::unique_ptr<char, decltype(&SDL_free)> basePath(SDL_GetBasePath(), SDL_free);
    if (!basePath) throw std::runtime_error(SDL_GetError());
    UseRuntimeDirectory(std::filesystem::u8path(basePath.get()));
    std::ofstream trace(output);
    if (!trace) throw std::runtime_error("Cannot create trace output");
    std::srand(1); // Identical CRT random sequence in both test processes only.
    auto context = Core::Context::GetInstance();
    const GlDebugLifetime glDebugLifetime;
    if (const char* cap = std::getenv("BOSHY_PROBE_FPS_CAP")) {
        const int value = std::stoi(cap);
        if (value != 30 && value != 60 && value != 120) throw std::runtime_error("Unsupported probe FPS cap");
        PTSD_Config::FPS_CAP = value; // Test process only; before Context::Update caches its interval.
    }
    Mix_MasterVolume(0);
    Mix_Volume(-1, 0);
    Mix_VolumeMusic(0);
    App app;
    app.Start();
    context->Update(); // Same initial Start -> Context ordering as the stock main.
    for (int frame = 0; frame < 24; ++frame) {
        if (frame == 0 || frame == 20) Key(SDL_SCANCODE_RETURN, true);
        if (frame == 2 || frame == 22) Key(SDL_SCANCODE_RETURN, false);
        context->Update();
        BoshyRegressionAccess::Begin();
        app.Update();
    }
    if (app.GetCurrentPhase() == App::GamePhase::MENU) throw std::runtime_error("Menu entry failed");
    BoshyRegressionAccess::Room(app, (scenario == "spikes" || scenario == "death-loop") ? "2" :
        scenario == "spider-bird" ? "8" : scenario == "enemy" ? "4_1" : (scenario == "boss" || scenario == "boss-death") ? "13" : "1");
    const auto floor = FindFloor(app, 1, scenario == "full-jump" || scenario == "short-jump" ||
        scenario == "double-jump" || scenario == "resize-play" ? 9 : 1);
    BoshyRegressionAccess::Place(app, floor);
    glm::vec2 fixture = floor;
    if (scenario == "oneway") fixture = FindFloor(app, 8, 1);
    if (scenario == "wall" || scenario == "head") fixture = FindContact(app, scenario == "head");
    if (scenario == "spikes" || scenario == "death-loop") fixture = FindTile(app, 5);
    if (scenario == "checkpoint") {
        const auto cp = BoshyRegressionAccess::FirstCheckpoint(app);
        if (!cp) throw std::runtime_error("Missing actual checkpoint fixture");
        fixture = cp->GetPosition() + glm::vec2(-60, 0);
    }
    int frames = scenario == "boss" ? 3300 : scenario == "boss-death" ? 1200 : scenario == "enemy" ? 300 : scenario == "death-loop" ? 450 : scenario == "reload-stress" ? 600 : scenario == "spider-bird" ? 900 : scenario == "resize-play" ? 350 : 150;
    if (scenario == "boss") fixture = {-500, -404};
    if (scenario == "spider-bird" || scenario == "resize-play") app.GodMode = true;
    if (scenario == "boss-death") app.GodMode = true; // Existing debug mode, explicitly a defeat/lifecycle workload.
    if (argc == 4) {
        if (const char* width = std::getenv("BOSHY_VISUAL_WIDTH")) {
            const char* height = std::getenv("BOSHY_VISUAL_HEIGHT");
            if (!height) throw std::runtime_error("Visual width needs height");
            SDL_SetWindowSize(SDL_GL_GetCurrentWindow(),std::stoi(width),std::stoi(height));
        }
    }
    int retirementSamples = 0, expiredSamples = 0;
    BoshyRegressionAccess::Header(trace);
    for (int frame = 0; frame < frames; ++frame) {
        BoshyRegressionAccess::Begin();
        if (scenario == "full-jump" || scenario == "short-jump" || scenario == "double-jump") {
            if (frame == 10) Key(SDL_SCANCODE_Z, true);
            if (frame == (scenario == "short-jump" ? 13 : scenario == "double-jump" ? 11 : 60)) Key(SDL_SCANCODE_Z, false);
            if (scenario == "double-jump") {
                if (frame == 18 || frame == 24) Key(SDL_SCANCODE_Z, true);
                if (frame == 19 || frame == 25) Key(SDL_SCANCODE_Z, false);
            }
        } else if (scenario == "movement") {
            if (frame == 10) Key(SDL_SCANCODE_RIGHT, true);
            if (frame == 20) Key(SDL_SCANCODE_RIGHT, false);
            if (frame == 30) Key(SDL_SCANCODE_LEFT, true);
            if (frame == 40) Key(SDL_SCANCODE_LEFT, false);
            if (frame == 50) { Key(SDL_SCANCODE_RIGHT, true); Key(SDL_SCANCODE_LEFT, true); }
            if (frame == 60) { Key(SDL_SCANCODE_RIGHT, false); Key(SDL_SCANCODE_LEFT, false); }
            if (frame == 70) Key(SDL_SCANCODE_UP, true);
            if (frame == 80) Key(SDL_SCANCODE_UP, false);
            if (frame == 90) Key(SDL_SCANCODE_DOWN, true);
            if (frame == 100) Key(SDL_SCANCODE_DOWN, false);
        } else if (scenario == "shooting" || scenario == "shoot-jump") {
            if (frame < 120) BoshyRegressionAccess::Place(app, {-560,364});
            if (frame == 10) { Key(SDL_SCANCODE_X, true); if (scenario == "shoot-jump") Key(SDL_SCANCODE_Z, true); }
            if (frame == 110) { Key(SDL_SCANCODE_X, false); if (scenario == "shoot-jump") Key(SDL_SCANCODE_Z, false); }
        } else if (scenario == "wall") {
            if (frame < 40) BoshyRegressionAccess::Place(app, fixture);
            if (frame == 0) Key(SDL_SCANCODE_RIGHT, true);
            if (frame == 40) Key(SDL_SCANCODE_RIGHT, false);
        } else if (scenario == "head") {
            if (frame == 2) Key(SDL_SCANCODE_Z, true);
            if (frame == 3) BoshyRegressionAccess::Place(app, fixture);
            if (frame == 10) Key(SDL_SCANCODE_Z, false);
        } else if (scenario == "death-loop") {
            if (frame % 150 == 0) BoshyRegressionAccess::Place(app, fixture);
            if (frame % 150 == 90) Key(SDL_SCANCODE_R, true);
            if (frame % 150 == 92) Key(SDL_SCANCODE_R, false);
        } else if (scenario == "spikes") {
            if (frame == 0) BoshyRegressionAccess::Place(app, fixture);
            if (frame == 90) Key(SDL_SCANCODE_R, true);
            if (frame == 92) Key(SDL_SCANCODE_R, false);
        } else if (scenario == "checkpoint") {
            if (frame < 95) BoshyRegressionAccess::Place(app, fixture);
            if (frame == 3) Key(SDL_SCANCODE_X, true);
            if (frame == 5) Key(SDL_SCANCODE_X, false);
            if (frame == 110) Key(SDL_SCANCODE_R, true);
            if (frame == 112) Key(SDL_SCANCODE_R, false);
        } else if (scenario == "transition") {
            if (frame == 5) BoshyRegressionAccess::Place(app, TileCenter(79,20));
            if (frame == 40) BoshyRegressionAccess::Place(app, TileCenter(0,20));
        } else if (scenario == "oneway") {
            if (frame < 4 || frame == 50) BoshyRegressionAccess::Place(app, fixture);
            if (frame == 4) Key(SDL_SCANCODE_Z, true);
            if (frame == 5) BoshyRegressionAccess::Place(app, fixture + glm::vec2(0,-32));
            if (frame == 20) Key(SDL_SCANCODE_Z, false);
        } else if (scenario == "resize-play") {
            auto* window = SDL_GL_GetCurrentWindow();
            if (frame == 10) Key(SDL_SCANCODE_RIGHT, true);
            if (frame == 20) Key(SDL_SCANCODE_RIGHT, false);
            if (frame == 40) Key(SDL_SCANCODE_Z, true);
            if (frame == 100) Key(SDL_SCANCODE_Z, false);
            if (frame == 120) Key(SDL_SCANCODE_X, true);
            if (frame == 200) Key(SDL_SCANCODE_X, false);
            if (frame == 50) SDL_SetWindowSize(window,1920,1080);
            if (frame == 90) SDL_SetWindowSize(window,2560,1440);
            if (frame == 130) SDL_SetWindowSize(window,3840,2160);
            if (frame == 180) SDL_SetWindowSize(window,960,540);
            if (frame == 225 && SDL_SetWindowFullscreen(window,SDL_WINDOW_FULLSCREEN_DESKTOP) != 0)
                throw std::runtime_error(SDL_GetError());
            if (frame == 275) {
                if (SDL_SetWindowFullscreen(window,0) != 0) throw std::runtime_error(SDL_GetError());
                SDL_SetWindowSize(window,1280,960);
            }
        } else if (scenario == "reload-stress") {
            if (frame % 10 == 0) {
                std::weak_ptr<CheckPoint> previous = BoshyRegressionAccess::FirstCheckpoint(app);
                BoshyRegressionAccess::Room(app, frame % 20 == 0 ? "2" : "1");
                ++retirementSamples;
                if (previous.expired()) ++expiredSamples;
#if BOSHY_CURRENT_REGRESSION
                if (!previous.expired()) throw std::runtime_error("Reload retained old checkpoint");
                const auto hash = BoshyRegressionAccess::MapHash(app);
                const auto position = BoshyRegressionAccess::Position(app);
                app.TeleportToMap("missing-room", "WORLD1");
                app.TeleportToMap("8", "WORLD2"); // Valid world index but no deployed World-2 map/image.
                if (BoshyRegressionAccess::MapHash(app) != hash || BoshyRegressionAccess::Position(app) != position)
                    throw std::runtime_error("Rejected teleport changed active room");
#endif
            }
        } else if (scenario == "spider-bird") {
            if (frame == 400) BoshyRegressionAccess::Room(app, "9");
            if (frame == 450 || frame == 660) BoshyRegressionAccess::Room(app, "8");
            if (frame == 600) BoshyRegressionAccess::Room(app, "12");
            BoshyRegressionAccess::Place(app, {400,-100});
        } else if (scenario == "boss-death") {
            if (frame < 450) BoshyRegressionAccess::Place(app, BoshyRegressionAccess::BossPosition(app) + glm::vec2(-120,0));
            if (frame == 10) Key(SDL_SCANCODE_X, true);
            if (frame == 440) Key(SDL_SCANCODE_X, false);
        } else if (scenario == "enemy" || scenario == "boss") {
            BoshyRegressionAccess::Place(app, scenario == "enemy" ? glm::vec2(0,-100) : fixture);
        } else if (scenario != "idle") {
            throw std::runtime_error("Unknown regression scenario: " + scenario);
        }
        context->Update(); // Poll a held transition before the next actual App update.
        app.Update();
        BoshyRegressionAccess::Write(trace, frame, app);
        if (argc == 4 && frame == std::stoi(argv[3])) {
            trace.flush();
            // Optional visual inspection holds this state; not used for parity measurements.
            if (const char* capture = std::getenv("BOSHY_CAPTURE_PATH"))
                CaptureGameFrame(std::filesystem::u8path(capture));
            else SDL_Delay(180000);
        }
    }
    trace.flush();
    if (!trace) throw std::runtime_error("Cannot flush trace output");
    std::cout << "RETIREMENT_SAMPLES=" << retirementSamples << " EXPIRED=" << expiredSamples << '\n';
    app.End();
    return 0;
} catch (const std::exception& error) {
    std::cerr << "Regression probe failed: " << error.what() << '\n';
    return 1;
}
