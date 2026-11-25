#include "MapObject/TippingStone.hpp"
#include "ImageRegion.hpp"
#include "Util/Image.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace {
constexpr int TileX = 50, TileY = 44, Width = 6, Height = 4;
constexpr float TipSeconds = 0.32f, TipAngle = 0.41887902f; // 24 degrees.
glm::vec2 Rotate(glm::vec2 v, float a) {
    return {v.x * std::cos(a) - v.y * std::sin(a),
            v.x * std::sin(a) + v.y * std::cos(a)};
}
}

std::shared_ptr<TippingStone> TippingStone::Create(const std::shared_ptr<MapInfoLoader>& map,
                                                 Util::Renderer& renderer) {
    // Validate the original footprint before specializing it; no tile-ID migration.
    if (map->GetCurrentPhase() != "9" || map->GetWidth() != 80 || map->GetHeight() != 60)
        throw std::runtime_error("WORLD1/Map9.txt: unexpected tipping-stone map");
    for (int x = 0; x < Width; ++x)
        if (map->GetTile(TileX+x, TileY) != 1)
            throw std::runtime_error("WORLD1/Map9.txt: missing stone surface at tile (50,44)");
    auto stone = std::make_shared<TippingStone>();
    for (int y = 0; y < Height; ++y)
        for (int x = 0; x < Width; ++x)
            stone->m_OriginalTiles[y*Width+x] = map->GetTile(TileX+x, TileY+y);
    stone->m_Drawable = std::make_shared<Util::Image>(RESOURCE_DIR "/Image/MapObject/platform.png");
    stone->m_Transform.translation = Home;
    stone->SetZIndex(-2);
    // The opaque platform pixels exactly match the baked stone. Cover only its
    // 96x64 rectangle with the corresponding original mountain background.
    stone->m_Backdrop = std::make_shared<Util::GameObject>(
        std::make_shared<ImageRegion>(RESOURCE_DIR "/Image/Background/WORLD1/phasenone.png",
                                      TileX*16, TileY*16, Width*16, Height*16), -9);
    stone->m_Backdrop->m_Transform.translation = Home;
    stone->AddChild(stone->m_Backdrop);
    renderer.AddChild(stone);
    return stone;
}

bool TippingStone::Trigger(float playerX, MapInfoLoader& map) {
    if (m_State != State::Idle) return false;
    m_State = State::Tipping;
    m_Direction = playerX < Home.x ? 1.0f : -1.0f;
    // Keep the opposite upper corner fixed while the loaded edge sinks first.
    m_Pivot = Home + glm::vec2(m_Direction * 48.0f, 32.0f);
    for (int y = 0; y < Height; ++y)
        for (int x = 0; x < Width; ++x)
            if (map.GetTile(TileX+x, TileY+y) == 1 || map.GetTile(TileX+x, TileY+y) == 2)
                map.SetTile(TileX+x, TileY+y, 0);
    return true;
}

void TippingStone::RestoreTiles(MapInfoLoader& map) const {
    for (int y = 0; y < Height; ++y)
        for (int x = 0; x < Width; ++x)
            map.SetTile(TileX+x, TileY+y, m_OriginalTiles[y*Width+x]);
}

void TippingStone::Update(float deltaTime) {
    if (m_State == State::Tipping) {
        m_Timer = std::min(TipSeconds, m_Timer + deltaTime);
        const float t = m_Timer / TipSeconds;
        m_Transform.rotation = m_Direction * TipAngle * t * t;
        m_Transform.translation = m_Pivot + Rotate(Home-m_Pivot, m_Transform.rotation);
        if (m_Timer >= TipSeconds) m_State = State::Falling;
    } else if (m_State == State::Falling) {
        m_FallSpeed += 1400.0f * deltaTime;
        m_Transform.translation.y -= m_FallSpeed * deltaTime;
        m_Transform.rotation += m_Direction * 0.75f * deltaTime;
        if (m_Transform.translation.y < -600.0f) {
            m_State = State::Gone;
            // Keep the separate background cover visible after the stone leaves.
            m_Drawable.reset();
        }
    }
}

std::optional<float> TippingStone::SurfaceAt(float playerX) const {
    if (m_State == State::Falling || m_State == State::Gone) return std::nullopt;
    const auto left = m_Transform.translation + Rotate({-48,32}, m_Transform.rotation);
    const auto right = m_Transform.translation + Rotate({48,32}, m_Transform.rotation);
    if (playerX < left.x || playerX > right.x) return std::nullopt;
    return left.y + (right.y-left.y) * ((playerX-left.x)/(right.x-left.x));
}
