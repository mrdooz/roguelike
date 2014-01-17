#include "entity.hpp"

using namespace rogue;

//-----------------------------------------------------------------------------
Entity::Entity()
    : _armor(0)
    , _armorBonus(0)
    , _weaponBonus(0)
    , _level(1)
    , _refCount(0)
{
}
//-----------------------------------------------------------------------------
Entity::~Entity()
{
}

//-----------------------------------------------------------------------------
void Entity::SetHealth(int health)
{
  _curHealth = max(0, health);
}

//-----------------------------------------------------------------------------
void Entity::SetArmorBonus(int bonus)
{
  _armorBonus = bonus;
}

//-----------------------------------------------------------------------------
void Entity::SetWeaponBonus(int bonus)
{
  _weaponBonus = bonus;
}
