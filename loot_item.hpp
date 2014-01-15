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

    LootItem(LootItem::Type type)
    {
      _type = type;
    }

    string ToString() const;

    Type _type;
    union
    {
      int _numGold;
      int _numPots;
      int _numUpgrades;
    };
  };
}
