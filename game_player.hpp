#pragma once
#include "game_state.hpp"
#include "event_manager.hpp"

namespace rogue
{
  struct SpellBase;

  class GamePlayer
  {
  public:
    GamePlayer();
    ~GamePlayer();

    void Update(GameState& gameState);
    bool OnKeyPressed(GameState& state, const Event& event);

  private:
    bool ValidMovement(GameState& state, const Event& event);
    bool ValidMultiPhaseAction(GameState& state, const Event& event);
    bool ValidSinglePhaseAction(GameState& state, const Event& event);

    typedef function<bool(GameState&, const Event&)> fnProcessAction;

    bool ProcessCharge(GameState& state, const Event& event);
    bool ProcessMightyBlow(GameState& state, const Event& event);

    map<PlayerAction, SpellBase*> _actionMap;
  };
}

