#include "player_factory.hpp"
#include "utils.hpp"

using namespace rogue;

PlayerFactory *PlayerFactory::_instance;

Player *PlayerFactory::CreatePlayer(PlayerClass pc)
{

  Player *player = new Player();
  player->_class = pc;

  switch (pc)
  {
    case PlayerClass::kWizard: 
      player->_intelligence = (int)gaussianRand(20, 3);
      player->_strength = (int)gaussianRand(10, 2);
      player->_dexterity = (int)gaussianRand(10, 2);
      player->_vitality = (int)gaussianRand(10, 2);
      player->_curMana = player->_maxMana = (int)gaussianRand(20, 2);
      break;

    case PlayerClass::kRogue: 
      player->_intelligence = (int)gaussianRand(10, 2); 
      player->_strength = (int)gaussianRand(10, 2); 
      player->_dexterity = (int)gaussianRand(20, 3); 
      player->_vitality = (int)gaussianRand(15, 2);
      player->_curMana = player->_maxMana = 0;
      break;

    case PlayerClass::kWarrior: 
      player->_intelligence = (int)gaussianRand(10, 2); 
      player->_strength = (int)gaussianRand(20, 3); 
      player->_dexterity = (int)gaussianRand(10, 2); 
      player->_vitality = (int)gaussianRand(20, 3);
      player->_curMana = player->_maxMana = 0;
      break;

    case PlayerClass::kCleric:
      player->_intelligence = (int)gaussianRand(20, 3); 
      player->_strength = (int)gaussianRand(10, 2); 
      player->_dexterity = (int)gaussianRand(10, 2); 
      player->_vitality = (int)gaussianRand(10, 2);
      player->_curMana = player->_maxMana = (int)gaussianRand(20, 2);
      break;
  }

  player->_curHealth = player->_maxHeath = (int)gaussianRand(10, 2);
  player->_level = 1;

  return player;
}

bool PlayerFactory::create() {
  assert(!_instance);
  _instance = new PlayerFactory();
  return true;
}

PlayerFactory &PlayerFactory::instance() {
  assert(_instance);
  return *_instance;
}

void PlayerFactory::close() {
  assert(_instance);
  delete exch_null(_instance);
}
