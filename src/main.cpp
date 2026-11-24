#include "App.hpp"

#include "Core/Context.hpp"
#include "RuntimePaths.hpp"

#include <iostream>
#include <memory>
#include <stdexcept>

int main(int, char**) try {
    const std::unique_ptr<char, decltype(&SDL_free)> basePath(SDL_GetBasePath(), SDL_free);
    if (!basePath) {
        throw std::runtime_error(std::string("Cannot locate executable: ") + SDL_GetError());
    }
    UseRuntimeDirectory(std::filesystem::u8path(basePath.get()));
    auto context = Core::Context::GetInstance();
    App app;

    while (!context->GetExit()) {
        switch (app.GetCurrentState()) {
            case App::State::START:
                app.Start();
                break;

            case App::State::UPDATE:
                app.Update();
                break;

            case App::State::END:
                app.End();
                context->SetExit(true);
                break;
        }
        context->Update();
    }
    return 0;
} catch (const std::exception& error) {
    std::cerr << "Boshy startup failed: " << error.what() << '\n';
    return 1;
}
