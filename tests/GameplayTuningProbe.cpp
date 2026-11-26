#include "GlDebugLifetime.hpp"
#include "FrameCapture.hpp"
#include "RegressionAccess.hpp"
#include "ImageRegion.hpp"
#include "Core/Context.hpp"
#include "RuntimePaths.hpp"
#include <cmath>
#include <fstream>
#include <memory>
#include <stdexcept>

namespace {
void Require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}
void Key(SDL_Scancode key, bool down) {
    SDL_Event event{};
    event.type = down ? SDL_KEYDOWN : SDL_KEYUP;
    event.key.state = down ? SDL_PRESSED : SDL_RELEASED;
    event.key.keysym.scancode = key;
    event.key.keysym.sym = SDL_GetKeyFromScancode(key);
    Require(SDL_PushEvent(&event) == 1, "SDL key injection failed");
}
}

int main(int argc, char** argv) try {
    Require(argc == 2, "Usage: GameplayTuningProbe OUTPUT_DIRECTORY");
    const auto output = std::filesystem::absolute(argv[1]);
    std::filesystem::create_directories(output);
    SDL_setenv("SDL_AUDIODRIVER", "dummy", 1);
    const std::unique_ptr<char, decltype(&SDL_free)> basePath(SDL_GetBasePath(), SDL_free);
    Require(basePath != nullptr, "No executable directory");
    UseRuntimeDirectory(std::filesystem::u8path(basePath.get()));
    std::srand(1);
    auto context = Core::Context::GetInstance();
    const GlDebugLifetime glDebugLifetime;
    if (const char* width = std::getenv("BOSHY_VISUAL_WIDTH")) {
        const char* height = std::getenv("BOSHY_VISUAL_HEIGHT");
        Require(height != nullptr, "Visual width requires height");
        SDL_SetWindowSize(SDL_GL_GetCurrentWindow(), std::stoi(width), std::stoi(height));
    }
    Mix_MasterVolume(0); Mix_Volume(-1, 0); Mix_VolumeMusic(0);
    App app;
    app.Start();
    context->Update();
    const auto step = [&] { context->Update(); BoshyRegressionAccess::Begin(); app.Update(); };
    for (int frame = 0; frame < 24; ++frame) {
        if (frame == 0 || frame == 20) Key(SDL_SCANCODE_RETURN, true);
        if (frame == 2 || frame == 22) Key(SDL_SCANCODE_RETURN, false);
        step();
    }
    Require(app.GetCurrentPhase() != App::GamePhase::MENU, "Menu entry failed");
    app.GodMode = true;
    std::ofstream trace(output / "tuning.csv");
    trace << "case|frame|x|y|angle|state|player_y\n";

    // Compare every position for two complete cycles against pre-change math.
    BoshyRegressionAccess::Room(app, "2");
    float elapsed = 0;
    for (int frame = 0; frame < 385; ++frame) {
        BoshyRegressionAccess::Place(app, {-610,-372});
        step();
        elapsed += PlayerManager::FrameSeconds;
        const auto p = BoshyRegressionAccess::TrapPositions(app);
        Require(p.x == std::round(-480 + std::sin(elapsed*2.0f)*480), "Floor sine changed");
        Require(p.y == std::round(160 + std::sin(elapsed*2.0f)*190), "Ceiling sine/phase changed");
        trace << "spikes|" << frame << '|' << p.x << '|' << p.y << "|||\n";
        if (frame == 46 || frame == 140)
            CaptureGameFrame(output / (frame == 46 ? "spikes-floor.ppm" : "spikes-ceiling.ppm"));
    }

    BoshyRegressionAccess::Room(app, "9");
    const auto originalTiles = BoshyRegressionAccess::Map(app)->GetMapData();
    auto stone = BoshyRegressionAccess::Stone(app);
    Require(stone && stone->GetState() == TippingStone::State::Idle, "Missing idle stone");
    BoshyRegressionAccess::ResetPlayer(app);
    BoshyRegressionAccess::Place(app, {208,-340}); // Below it, with sprite clear of the crop.
    step();
    Require(stone->GetState() == TippingStone::State::Idle, "Underside triggered stone");
    CaptureGameFrame(output / "stone-idle.ppm");
    BoshyRegressionAccess::ResetPlayer(app);
    BoshyRegressionAccess::Place(app, {228,-180}); // Actual fall onto the unchanged tile surface.
    int firstTip = -1, firstFall = -1, firstGone = -1, supports = 0;
    bool tipCaptured = false, fallCaptured = false;
    for (int frame = 0; frame < 95; ++frame) {
        if (stone->GetState() == TippingStone::State::Falling || stone->GetState() == TippingStone::State::Gone)
            BoshyRegressionAccess::Place(app, {-500,-200});
        step();
        const auto state = stone->GetState();
        const auto p = stone->GetPosition();
        const auto player = app.GetRuntimeView();
        if (state == TippingStone::State::Tipping) {
            if (firstTip < 0) firstTip = frame;
            Require(stone->GetAngle() < 0, "Loaded right edge should tip down first");
            const auto surface = stone->SurfaceAt(player.position.x);
            if (surface && player.velocityY == 0) {
                Require(std::abs(player.position.y - *surface - 12) < .001f, "Support did not follow slope");
                ++supports;
            }
            if (!tipCaptured && std::abs(stone->GetAngle()) > .2f) {
                CaptureGameFrame(output / "stone-tipping.ppm"); tipCaptured = true;
            }
        }
        if (state == TippingStone::State::Falling && firstFall < 0) firstFall = frame;
        if (state == TippingStone::State::Falling && !fallCaptured && p.y < -330) {
            CaptureGameFrame(output / "stone-falling.ppm"); fallCaptured = true;
        }
        if (state == TippingStone::State::Gone && firstGone < 0) {
            firstGone = frame; CaptureGameFrame(output / "stone-gone.ppm");
        }
        trace << "stone|" << frame << '|' << p.x << '|' << p.y << '|' << stone->GetAngle()
              << '|' << static_cast<int>(state) << '|' << player.position.y << '\n';
        const auto& tiles = BoshyRegressionAccess::Map(app)->GetMapData();
        for (int y=0; y<60; ++y) for (int x=0; x<80; ++x) {
            const bool footprint = x>=50 && x<56 && y>=44 && y<48;
            const int before = originalTiles[y][x];
            const int expected = firstTip>=0 && footprint && (before==1 || before==2) ? 0 : before;
            Require(tiles[y][x] == expected, "Unexpected map edit / invisible stone floor");
        }
    }
    Require(firstTip >= 0 && firstFall-firstTip == 19 && firstGone>firstFall && supports>=15,
            "Missing landing, 0.32s tip, supported slope, or complete fall");
    Require(tipCaptured && fallCaptured, "Missing rendered tipping/falling stages");
    Require(!stone->SurfaceAt(208), "Gone stone still collides");

    // Real R reload and repeat entry must restore the exact map and release both layers.
    std::weak_ptr<TippingStone> retired = stone;
    stone.reset();
    Key(SDL_SCANCODE_R, true); step(); Key(SDL_SCANCODE_R, false); step();
    Require(retired.expired(), "R retained old stone/cover");
    Require(BoshyRegressionAccess::Map(app)->GetMapData() == originalTiles, "R did not restore map");
    for (int cycle = 0; cycle < 12; ++cycle) {
        BoshyRegressionAccess::ResetPlayer(app);
        BoshyRegressionAccess::Place(app, {188,-212});
        step();
        Require(BoshyRegressionAccess::Stone(app)->GetAngle()>0, "Loaded left edge should tip down");
        app.ReloadMapObjects();
        Require(BoshyRegressionAccess::Map(app)->GetMapData() == originalTiles, "Repeat reload lost original cells");
    }
    BoshyRegressionAccess::ResetPlayer(app);
    BoshyRegressionAccess::Place(app, {228,-212});
    step();
    Key(SDL_SCANCODE_Z, true); step();
    Require(app.GetRuntimeView().velocityY>0, "Tipping support cancelled jump");
    const float jumpY = app.GetRuntimeView().position.y;
    Key(SDL_SCANCODE_Z, false); step();
    Require(app.GetRuntimeView().position.y>jumpY, "Player could not jump off tipping surface");
    retired = BoshyRegressionAccess::Stone(app);
    BoshyRegressionAccess::Room(app,"1"); step();
    Require(retired.expired() && !BoshyRegressionAccess::Stone(app), "Room change retained stone");
    BoshyRegressionAccess::Room(app,"9");
    Require(BoshyRegressionAccess::Map(app)->GetMapData() == originalTiles, "Room re-entry changed map");

    CheckPoint reaction;
    Require(!reaction.ShouldShootBullet(), "Inactive checkpoint fired");
    reaction.play();
    for (int frame=1; frame<=30; ++frame) {
        reaction.UpdateBulletShootDelay(PlayerManager::FrameSeconds);
        Require(reaction.ShouldShootBullet() == (frame==30), "Checkpoint reaction is not 0.5 simulation seconds");
    }
    reaction.ResetShotStatus();
    reaction.UpdateBulletShootDelay(1.0f);
    Require(!reaction.ShouldShootBullet(), "Reset checkpoint fired again");
    reaction.play();
    reaction.UpdateBulletShootDelay(.25f);
    reaction.play(); // Another hit retains the original timer-reset behavior.
    reaction.UpdateBulletShootDelay(.49f);
    Require(!reaction.ShouldShootBullet(), "Repeated checkpoint hit did not reset delay");
    reaction.UpdateBulletShootDelay(.01f);
    Require(reaction.ShouldShootBullet(), "Checkpoint missed 0.5-second boundary");

    Util::Renderer bullets;
    auto counter = Bullet::CreateCheckpointBullet({0,0}, {3,4}, 1, bullets);
    for (int frame=0; frame<30; ++frame) {
        const auto before = counter->GetPosition();
        Require(counter->UpdateCheckpointBullet(PlayerManager::FrameSeconds), "Counter-shot expired early");
        const auto delta = counter->GetPosition()-before;
        Require(std::abs(glm::length(delta)-8)<.0001f, "Counter-shot original speed 8 changed");
    }
    auto zero = Bullet::CreateCheckpointBullet({0,0}, {0,0}, 1, bullets);
    zero->UpdateCheckpointBullet(PlayerManager::FrameSeconds);
    Require(zero->GetPosition() == glm::vec2(8,0), "Zero-distance aim fallback changed");
    auto expired = Bullet::CreateCheckpointBullet({0,0}, {100,0}, .01f, bullets);
    Require(!expired->UpdateCheckpointBullet(PlayerManager::FrameSeconds) && expired->GetPosition()==glm::vec2(0),
            "Expired counter-shot moved");
    auto outside = Bullet::CreateCheckpointBullet({635,0}, {1000,0}, 1, bullets);
    Require(!outside->UpdateCheckpointBullet(PlayerManager::FrameSeconds), "Offscreen counter-shot survived");

    for (const auto* path : {"Resources/absent.png", "Resources/Image/MapObject/platform.png"}) {
        bool rejected = false;
        try { ImageRegion invalid(path, 0, 0, 10000, 64); }
        catch (const std::runtime_error& error) { rejected = std::string(error.what()).find(path) != std::string::npos; }
        Require(rejected, "Missing/out-of-bounds crop not diagnosed with path");
    }
    trace.flush(); Require(bool(trace), "Cannot write tuning trace");
    std::ofstream result(output / "result.json");
    result << "{\"result\":\"PASS\",\"audio\":\"dummy\",\"spike_frames\":385,"
           << "\"stone_first_tip\":" << firstTip << ",\"stone_first_fall\":" << firstFall
           << ",\"stone_first_gone\":" << firstGone << ",\"supported_tip_frames\":" << supports
           << ",\"reload_cycles\":12,\"counter_speed_before\":8,\"counter_speed_after\":8,"
           << "\"counter_delay_before\":1,\"counter_delay_after\":0.5,\"reaction_updates\":30}\n";
    app.End();
    return 0;
} catch (const std::exception& error) {
    std::cerr << "Gameplay tuning probe FAILED: " << error.what() << '\n'; return 1;
}
