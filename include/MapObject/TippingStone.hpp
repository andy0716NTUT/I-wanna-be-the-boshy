#pragma once

#include "MapInfoLoader.hpp"
#include "Util/GameObject.hpp"
#include "Util/Renderer.hpp"
#include <optional>
#include <array>

// Only WORLD1 room 9's right-hand stepping stone, before the heart blocks.
class TippingStone final : public Util::GameObject {
public:
    enum class State { Idle, Tipping, Falling, Gone };
    static std::shared_ptr<TippingStone> Create(const std::shared_ptr<MapInfoLoader>& map,
                                               Util::Renderer& renderer);
    bool Trigger(float playerX, MapInfoLoader& map);
    void RestoreTiles(MapInfoLoader& map) const;
    void Update(float deltaTime);
    std::optional<float> SurfaceAt(float playerX) const;
    State GetState() const { return m_State; }
    glm::vec2 GetPosition() const { return m_Transform.translation; }
    float GetAngle() const { return m_Transform.rotation; }
    static constexpr glm::vec2 Home{208.0f, -256.0f};
private:
    State m_State = State::Idle;
    float m_Timer = 0.0f;
    float m_Direction = -1.0f;
    float m_FallSpeed = 120.0f;
    glm::vec2 m_Pivot{};
    std::shared_ptr<Util::GameObject> m_Backdrop;
    std::array<int,24> m_OriginalTiles{};
};
