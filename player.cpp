#include "player.hpp"
#include "utils.hpp"

using namespace rogue;

string rogue::playerClassToString(PlayerClass pc)
{
  switch (pc)
  {
  case PlayerClass::kWizard: return "Wizard";
  case PlayerClass::kRanger: return "Ranger";
  case PlayerClass::kWarrior: return "Warrior";
  case PlayerClass::kCleric: return "Cleric";
  default: assert(false); return "";
  }
}

string rogue::playerModeToString(PlayerMode pm)
{
  switch (pm)
  {
  case PlayerMode::kExplore: return "Explore";
  case PlayerMode::kGuard: return "Guard";
  case PlayerMode::kFollow: return "Follow";
  case PlayerMode::kInteractive: return "Interactive";
  default: assert(false); return "";
  }
}

Player::Player()
  : Entity(Entity::Type::Player)
  , _chargeRange(3)
  , _heading(Heading::East)
  , _mode(PlayerMode::kInteractive)
  , _gold(0)
  , _manaPotions(0)
  , _healthPotions(0)
  , _xp(0)
  , _xpForNextLevel(10)
  , _hasMoved(false)
{
}
