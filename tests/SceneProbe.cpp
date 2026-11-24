#include "App.hpp"
#include "Core/Context.hpp"
#include "RuntimePaths.hpp"

#include <array>
#include <fstream>
#include <memory>
#include <stdexcept>

namespace {
struct InputFrame { int frame; SDL_Scancode key; bool down; };
constexpr std::array<InputFrame, 16> inputs{{
    {0, SDL_SCANCODE_RETURN, true}, {2, SDL_SCANCODE_RETURN, false},
    {20, SDL_SCANCODE_RETURN, true}, {22, SDL_SCANCODE_RETURN, false},
    {50, SDL_SCANCODE_RIGHT, true}, {60, SDL_SCANCODE_RIGHT, false},
    {80, SDL_SCANCODE_Z, true}, {100, SDL_SCANCODE_Z, false},
    {140, SDL_SCANCODE_Z, true}, {143, SDL_SCANCODE_Z, false},
    {180, SDL_SCANCODE_X, true}, {240, SDL_SCANCODE_X, false},
    {260, SDL_SCANCODE_R, true}, {262, SDL_SCANCODE_R, false},
    {280, SDL_SCANCODE_LEFT, true}, {290, SDL_SCANCODE_LEFT, false}
}};
}

int main(int argc, char** argv) try {
    if (argc != 2) throw std::runtime_error("Expected absolute trace output path");
    // Resolve output before selecting the executable's resource directory.
    const auto output = std::filesystem::absolute(argv[1]);
    const std::unique_ptr<char, decltype(&SDL_free)> basePath(SDL_GetBasePath(), SDL_free);
    if (!basePath) throw std::runtime_error(SDL_GetError());
    UseRuntimeDirectory(std::filesystem::u8path(basePath.get()));
    std::ofstream trace(output);
    if (!trace) throw std::runtime_error("Cannot create trace: " + output.u8string());
    trace << "frame|phase|world|position_before_update|tile_before_update|right|left|z|x|r|elapsed_ms\n";
    auto context = Core::Context::GetInstance();
    App app;
    app.Start();
    for (int frame = 0; frame < 320; ++frame) {
        // Preserve the real loop order: App update, then input poll/swap/delay.
        app.Update();
        trace << frame << '|' << static_cast<int>(app.GetCurrentPhase()) << '|'
              << app.CurrentWorld << '|' << app.m_DebugInfo.positionInfo << '|'
              << app.m_DebugInfo.tileInfo << '|'
              << Util::Input::IsKeyPressed(Util::Keycode::RIGHT) << '|'
              << Util::Input::IsKeyPressed(Util::Keycode::LEFT) << '|'
              << Util::Input::IsKeyPressed(Util::Keycode::Z) << '|'
              << Util::Input::IsKeyPressed(Util::Keycode::X) << '|'
              << Util::Input::IsKeyPressed(Util::Keycode::R) << '|'
              << Util::Time::GetElapsedTimeMs() << '\n';
        for (const auto& input : inputs) {
            if (input.frame != frame) continue;
            SDL_Event event{};
            event.type = input.down ? SDL_KEYDOWN : SDL_KEYUP;
            event.key.state = input.down ? SDL_PRESSED : SDL_RELEASED;
            event.key.keysym.scancode = input.key;
            event.key.keysym.sym = SDL_GetKeyFromScancode(input.key);
            if (SDL_PushEvent(&event) != 1) throw std::runtime_error(SDL_GetError());
        }
        context->Update();
    }
    if (app.GetCurrentPhase() == App::GamePhase::MENU) {
        throw std::runtime_error("Scripted menu entry failed; no gameplay measurement valid");
    }
    trace.flush();
    if (!trace) throw std::runtime_error("Failed writing scene trace");
    app.End();
    return 0;
} catch (const std::exception& error) {
    std::cerr << "Scene probe failed: " << error.what() << '\n';
    return 1;
}
