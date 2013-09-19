#include "precompiled.hpp"
#include "player.hpp"
#include "utils.hpp"

using namespace rogue;

string playerClassToString(PlayerClass pc) {
  switch (pc) {
  case PlayerClass::kWizard: return "Wizard";
  case PlayerClass::kRogue: return "Rogue";
  case PlayerClass::kWarrior: return "Warrior";
  case PlayerClass::kCleric: return "Cleric";
  default: assert(false); return "";
  }
}

string playerModeToString(PlayerMode pm) {
  switch (pm) {
  case PlayerMode::kExplore: return "Explore";
  case PlayerMode::kGuard: return "Guard";
  case PlayerMode::kFollow: return "Follow";
  case PlayerMode::kInteractive: return "Interactive";
  default: assert(false); return "";
  }
}

