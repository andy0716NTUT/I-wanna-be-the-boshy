
#include "MapObject/phase2trap.hpp"
#include "MapObject/bird.hpp"
#include "Util/Renderer.hpp"
#include "MapInfoLoader.hpp"
#include "GameObjectHelper.hpp"
class TrapManager {
public:
  void Initialize(Util::Renderer& root, std::shared_ptr<MapInfoLoader> mapLoader);
  void Update(float deltaTime, const std::string& phase, const glm::vec2& boshyPos, bool godMode, glm::vec2& respawnPos, int& currentX, int& currentY);
  void Clear();

private:
  std::shared_ptr<phase2trap> m_phase2trap_down;
  std::shared_ptr<phase2trap> m_phase2trap_up;
  std::shared_ptr<Bird> m_phase8bird;

  bool trapCreated = false;
  float birdTimer = 0.0f;

  Util::Renderer* m_root = nullptr;
  std::shared_ptr<MapInfoLoader> m_mapLoader = nullptr;
};