#include "player_factory.hpp"
#include "utils.hpp"

using namespace rogue;

Player *PlayerFactory::CreatePlayer(PlayerClass pc, const string& name)
{

  Player *player = new Player();
  player->_class = pc;
  player->_name = name;

  switch (pc)
  {
    case PlayerClass::kWizard: 
      player->_intelligence = (int)gaussianRand(20, 3);
      player->_strength = (int)gaussianRand(10, 2);
      player->_dexterity = (int)gaussianRand(10, 2);
      player->_vitality = (int)gaussianRand(10, 2);
      player->_curMana = player->_maxMana = (int)gaussianRand(20, 2);
      break;

    case PlayerClass::kRanger:
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

  player->_curHealth = player->_maxHealth = (int)gaussianRand(10, 2);
  player->_level = 1;

  return player;
}
