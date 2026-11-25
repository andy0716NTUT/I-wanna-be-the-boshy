# Game translation units actually compiled by eeb1f72 (excluding its main).
# Keep this list independent of the refactored target: the unused original
# PlayerManager prototype must never be enabled accidentally in the reference.
set(BOSHY_ORIGINAL_GAME_SOURCES
    src/App.cpp src/AppStart.cpp src/AppUpdate.cpp
    src/MapInfoLoader.cpp src/ResourceManager.cpp src/CheckPoint.cpp src/imgui.cpp
    src/Character.cpp src/AnimatedCharacter.cpp src/Bullet.cpp src/bear.cpp
    src/phase2trap.cpp src/Bird.cpp src/BgmManager.cpp
    src/Boss1/Boss1.cpp src/Boss1/BulletTypeA.cpp src/Boss1/BulletTypeB.cpp
    src/Boss1/BulletTypeC.cpp src/Boss1/LightAttack.cpp src/Boss1/BossDeadAnimation.cpp
    src/spider.cpp src/Menu.cpp src/GameOverUI.cpp)
