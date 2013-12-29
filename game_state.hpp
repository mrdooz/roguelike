#pragma once

namespace rogue
{
  class Level;
  class Party;

  class Player;
  class Monster;

  class GameState
  {
  public:
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
