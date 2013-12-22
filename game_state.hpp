#ifndef _GAME_STATE_HPP_
#define _GAME_STATE_HPP_

#include "types.hpp"
#include "roguelike.hpp"

namespace rogue
{
  class Level;
  class Party;

  class Player;
  class Monster;

  struct GameState
  {
    GameState();
    ~GameState();
    Player* GetActivePlayer() const;
    size_t _activePlayer;
    Level *_level;
    Party *_party;
    bool _twoPhaseAction;
    bool _monsterPhase;
  };

  void UpdateState(GameState& state, Event* event);
}

#endif
