#define SDL_MAIN_HANDLED
#include "GameObjectHelper.hpp"
#include <stdexcept>

int main() {
    const auto check = [](bool ok) { if (!ok) throw std::runtime_error("Renderer retained retired object"); };
    Util::Renderer renderer;
    for (int reload = 0; reload < 500; ++reload) {
        auto object = std::make_shared<Util::GameObject>();
        std::weak_ptr<Util::GameObject> retired = object;
        renderer.AddChild(object);
        renderer.AddChild(object); // Existing accidental duplicate registration.
        ClearGameObjects(object, renderer);
        check(!object && retired.expired());
        ClearGameObjects(object, renderer); // Repeated cleanup is safe.

        std::vector<std::shared_ptr<Util::GameObject>> objects{std::make_shared<Util::GameObject>(), nullptr};
        retired = objects.front();
        renderer.AddChild(objects.front());
        ClearGameObjects(objects, renderer);
        check(objects.empty() && retired.expired());
    }
    return 0;
}
