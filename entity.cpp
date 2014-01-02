#include "entity.hpp"

using namespace rogue;

Entity::Entity(Type type)
    : _armor(0)
    , _armorBonus(0)
    , _weaponBonus(0)
    , _type(type)
{
}

void Entity::SetHealth(int health)
{
  _curHealth = max(0, health);
}
