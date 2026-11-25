#pragma once

#include "App.hpp"
#include <algorithm>
#include <cstdint>
#include <iomanip>
#include <limits>
#include <sstream>
#include <utility>

template<class F> struct BoshyRegressionScope {
    explicit BoshyRegressionScope(F action) : action(std::move(action)) {}
    ~BoshyRegressionScope() { action(); }
    F action;
};

// This friend exists only in generated test copies. No normal-game layout/API change.
struct BoshyRegressionAccess {
    inline static float velocity = 0, cooldown = 0;
    inline static int jumps = 0;
    inline static bool jumping = false;
    inline static std::string events;
    static void Physics(float vy, int count, float cd, bool inJump) {
        velocity = vy; jumps = count; cooldown = cd; jumping = inJump;
    }
    static void Event(const char* name) { if (!events.empty()) events += ','; events += name; }
    static void Begin() { events.clear(); }
    static glm::vec2 Position(const App& app) { return app.m_Boshy->GetPosition(); }
    static int Death(const App& app) { return static_cast<int>(app.deathType); }
    static void Place(App& app, glm::vec2 position) { app.m_Boshy->SetPosition(position); }
    static void Room(App& app, const std::string& phase, const std::string& world = "WORLD1") {
        app.TeleportToMap(phase, world);
    }
    static std::shared_ptr<MapInfoLoader> Map(App& app) { return app.m_MapLoader; }
    static glm::vec2 Checkpoint(const App& app) { return app.currentCheckPoint; }
    static glm::vec2 BossPosition(const App& app) {
        return app.m_Boss1 ? app.m_Boss1->GetPosition() : glm::vec2(500,-300);
    }
    static std::shared_ptr<CheckPoint> FirstCheckpoint(App& app) {
        return app.m_CheckPoints.empty() ? nullptr : app.m_CheckPoints.front();
    }
    static std::uint64_t MapHash(const App& app) {
        std::uint64_t hash = 14695981039346656037ULL;
        for (const auto& row : app.m_MapLoader->GetMapData()) for (int tile : row) {
            hash ^= static_cast<std::uint32_t>(tile); hash *= 1099511628211ULL;
        }
        return hash;
    }
    static std::string Bullets(const std::vector<std::shared_ptr<Bullet>>& bullets) {
        std::ostringstream out;
        out << std::setprecision(std::numeric_limits<float>::max_digits10);
        for (const auto& bullet : bullets) if (bullet) {
            const auto p = bullet->GetPosition();
            out << p.x << ':' << p.y << ':' << bullet->IsVisible() << ':' << bullet->lifeTime << ';';
        }
        return out.str();
    }
    static std::string Boss(const App& app) {
        if (!app.m_Boss1) return "none";
        const auto& boss = *app.m_Boss1;
        std::ostringstream out;
        out << std::setprecision(std::numeric_limits<float>::max_digits10)
            << static_cast<int>(boss.m_AttackType) << ':' << boss.currentHp << ':'
            << boss.m_Transform.translation.x << ':' << boss.m_Transform.translation.y << ':'
            << boss.TypeAShootCount << ':' << boss.m_BulletsA.size() << ':'
            << boss.m_LightAttackCount << ':' << boss.TypeBShootCount << ':'
            << boss.m_BulletsB.size() << ':' << boss.m_BulletsC.size() << ':' << boss.isPlayerDead;
        for (const auto& bullet : boss.m_BulletsA) out << ";A:" << bullet->GetPosition().x << ':' << bullet->GetPosition().y;
        for (const auto& bullet : boss.m_BulletsB) out << ";B:" << bullet->GetPosition().x << ':' << bullet->GetPosition().y;
        for (const auto& bullet : boss.m_BulletsC) out << ";C:" << bullet->GetPosition().x << ':' << bullet->GetPosition().y;
        return out.str();
    }
    static void Header(std::ostream& out) {
        out << "frame|x|y|vy|jumps|cooldown|jumping|death|player_visible|death_ui|ui_timer|ui_scale|"
               "world|room|loaded_room|cp_x|cp_y|cp_room|bullets|cp_bullets|map_hash|events|boss|bear|actors|input\n";
    }
    static void Write(std::ostream& out, int frame, const App& app) {
        const auto p = Position(app);
        out << std::setprecision(std::numeric_limits<float>::max_digits10)
            << frame << '|' << p.x << '|' << p.y << '|' << velocity << '|' << jumps << '|'
            << cooldown << '|' << jumping << '|' << Death(app) << '|'
            << app.m_Boshy->m_Visible << '|' << app.m_GameOverUI->m_Visible << '|'
            << app.m_GameOverUI->m_Timer << '|' << app.m_GameOverUI->m_Transform.scale.x << '|'
            << app.CurrentWorld << '|' << app.CurrentPhase << '|' << app.m_MapLoader->GetCurrentPhase() << '|'
            << app.currentCheckPoint.x << '|' << app.currentCheckPoint.y << '|' << app.currentCheckPointPhase << '|'
            << Bullets(app.m_Bullets) << '|' << Bullets(app.m_CheckpointBullets) << '|'
            << MapHash(app) << '|' << events << '|' << Boss(app) << '|';
        if (app.m_bear && app.m_bear->exist()) out << app.m_bear->GetPosition().x << ':' << app.m_bear->GetPosition().y;
        else out << "none";
        out << '|' << app.m_Platform.size() << ':' << app.m_CheckPoints.size() << ':' << app.m_FallingGround.size()
            << ':' << bool(app.m_phase2trap_up) << ':' << bool(app.m_phase8spider) << ':' << bool(app.m_phase8bird) << '|';
        for (const auto key : {Util::Keycode::RIGHT,Util::Keycode::LEFT,Util::Keycode::UP,Util::Keycode::DOWN,
                               Util::Keycode::Z,Util::Keycode::X,Util::Keycode::R})
            out << Util::Input::IsKeyPressed(key);
        out << '\n';
    }
};
