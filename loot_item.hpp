#pragma once

namespace rogue
{
  struct LootItem
  {
    enum class Type
    {
      Gold,
      ManaPotion,
      HealthPotion,
      WeaponUpgrade,
      ArmorUpgrade,
    };

    Type _type;
    union
    {
      int _numGold;
      int _numPots;
      int _numUpgrades;
    };
  };

  // Meh, kinda cheesy just because I want to store LootItems in a union, so I can't
  // have a default ctor
  struct LootItemEx : public LootItem
  {
    LootItemEx(LootItem::Type type)
    {
      _type = type;
    }
  };
}
