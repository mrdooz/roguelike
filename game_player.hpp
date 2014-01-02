#pragma once
#include "game_state.hpp"
#include "event_manager.hpp"

namespace rogue
{
  struct SpellBase;

  class GamePlayer
  {
  public:
    typedef function<int (const GameState&, int, int)> fnTileAtPos;

    GamePlayer(const fnTileAtPos& fnTileAtPos);
    ~GamePlayer();

    void Update(GameState& gameState);
    bool OnKeyPressed(GameState& state, const Event& event);
    bool OnMouseButtonReleased(GameState& state, const Event& event);

  private:
    bool ValidMovement(GameState& state, const Event& event);
    bool ValidMultiPhaseAction(GameState& state, const Event& event);
    bool ValidSinglePhaseAction(GameState& state, const Event& event);

    typedef function<bool(GameState&, const Event&)> fnProcessAction;

    fnTileAtPos _fnTileAtPos;
    map<PlayerAction, SpellBase*> _actionMap;
  };
}

