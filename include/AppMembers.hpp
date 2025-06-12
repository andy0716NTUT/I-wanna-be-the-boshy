// AppMembers.hpp
#pragma once

// 死亡类型枚举
enum class DeathType {
    NONE,       // 没有死亡
    REAL_DEATH, // 真正的死亡（需要死亡动画）
    RESET       // 按R键重置（不需要死亡动画）
};

glm::vec2 currentCheckPoint;
std::string currentCheckPointPhase;
std::string CurrentPhase;
std::string checkPointWorld;
int currentX, currentY;
int checkPointX, checkPointY;

std::shared_ptr<World> m_World;
State m_CurrentState;
GamePhase m_GamePhase;
Util::Renderer m_Root;

std::shared_ptr<BgmManager> m_BGM;
std::shared_ptr<Menu> m_Menu;
std::shared_ptr<MapInfoLoader> m_MapLoader;
std::shared_ptr<AnimatedCharacter> m_Boshy;
std::shared_ptr<Bullet> m_Bullet;
std::shared_ptr<ResourceManager> m_PRM;
std::shared_ptr<phase2trap> m_phase2trap_up;
std::shared_ptr<phase2trap> m_phase2trap_down;
std::shared_ptr<Bird> m_phase8bird;
std::shared_ptr<bear> m_bear;
std::shared_ptr<Boss1> m_Boss1;
std::shared_ptr<spider> m_phase8spider;

std::vector<std::shared_ptr<Bullet>> m_Bullets;
std::vector<std::shared_ptr<Bullet>> m_CheckpointBullets; // 檢查點發射的子彈
std::vector<std::shared_ptr<CheckPoint>> m_CheckPoints;
std::vector<std::shared_ptr<JumpBoost>> m_jumpBoost;
std::vector<std::shared_ptr<FallingGround>> m_FallingGround;
std::vector<std::shared_ptr<Platform>> m_Platform;
std::shared_ptr<GameOverUI> m_GameOverUI;
bool trapCreated;
DeathType deathType = DeathType::NONE; // 替换原来的isDead
float deathAnimTimer = 0.0f; // 死亡动画计时器
bool deathAnimFinished = false; // 死亡动画是否完成
float shootCooldown;
bool NotFirstTryBoss;
