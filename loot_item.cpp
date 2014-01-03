#include "loot_item.hpp"
#include "utils.hpp"

using namespace rogue;

string LootItem::ToString() const
{
  switch (_type)
  {
    case Type::Gold: return toString("%d gold", _numGold);
    case Type::ManaPotion: return toString("%d mana potion%s", _numPots, _numPots > 1 ? "s" : "");
    case Type::HealthPotion: return toString("%d health potion%s", _numPots, _numPots > 1 ? "s" : "");
    case Type::WeaponUpgrade: return toString("%d weapon upgrade%s", _numUpgrades, _numUpgrades > 1 ? "s" : "");
    case Type::ArmorUpgrade: return toString("%d armor upgrade%s", _numUpgrades, _numUpgrades > 1 ? "s" : "");
  }

  return "";
}
