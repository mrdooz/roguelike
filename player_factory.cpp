#include "precompiled.hpp"
#include "player_factory.hpp"
#include "utils.hpp"

PlayerFactory *PlayerFactory::_instance;

Player *PlayerFactory::createPlayer(PlayerClass pc) {
  Player *player = new Player();
  player->_class = pc;

  switch (pc) {

  case PlayerClass::kWizard: 
    player->_intelligence = (int)gaussianRand(20, 3);
    player->_strength = (int)gaussianRand(10, 2);
    player->_dexterity = (int)gaussianRand(10, 2);
    player->_vitality = (int)gaussianRand(10, 2);
    player->_sprite.setTextureRect(sf::IntRect(0, 2*8, 8, 8));
    break;

  case PlayerClass::kRogue: 
    player->_intelligence = (int)gaussianRand(10, 2); 
    player->_strength = (int)gaussianRand(10, 2); 
    player->_dexterity = (int)gaussianRand(20, 3); 
    player->_vitality = (int)gaussianRand(15, 2);
    player->_sprite.setTextureRect(sf::IntRect(3*8, 0, 8, 8));
    break;

  case PlayerClass::kWarrior: 
    player->_intelligence = (int)gaussianRand(10, 2); 
    player->_strength = (int)gaussianRand(20, 3); 
    player->_dexterity = (int)gaussianRand(10, 2); 
    player->_vitality = (int)gaussianRand(20, 3);
    player->_sprite.setTextureRect(sf::IntRect(15*8, 0, 8, 8));
    break;

  case PlayerClass::kCleric:
    player->_intelligence = (int)gaussianRand(20, 3); 
    player->_strength = (int)gaussianRand(10, 2); 
    player->_dexterity = (int)gaussianRand(10, 2); 
    player->_vitality = (int)gaussianRand(10, 2);
    player->_sprite.setTextureRect(sf::IntRect(6*8, 0, 8, 8));
    break;
  }
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
