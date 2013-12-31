#include "player.hpp"
#include "utils.hpp"

using namespace rogue;

string rogue::playerClassToString(PlayerClass pc)
{
  switch (pc) {
  case PlayerClass::kWizard: return "Wizard";
  case PlayerClass::kRanger: return "Ranger";
  case PlayerClass::kWarrior: return "Warrior";
  case PlayerClass::kCleric: return "Cleric";
  default: assert(false); return "";
  }
}

string rogue::playerModeToString(PlayerMode pm)
{
  switch (pm) {
  case PlayerMode::kExplore: return "Explore";
  case PlayerMode::kGuard: return "Guard";
  case PlayerMode::kFollow: return "Follow";
  case PlayerMode::kInteractive: return "Interactive";
  default: assert(false); return "";
  }
}

Player::Player()
  : _chargeRange(3)
  , _heading(Heading::Right)
  , _mode(PlayerMode::kInteractive)
  , _hasMoved(false)
{
}
