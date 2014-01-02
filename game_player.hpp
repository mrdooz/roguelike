#pragma once
#include "game_state.hpp"
#include "game_event_manager.hpp"
#include "window_event_manager.hpp"

namespace rogue
{
  struct SpellBase;

  class GamePlayer
  {
  public:
    typedef function<int (const GameState&, int, int)> fnTileAtPos;

    GamePlayer(const fnTileAtPos& fnTileAtPos);
    ~GamePlayer();

    bool Init();

    void Update(GameState& gameState);
    bool OnKeyPressed(const Event& event);
    bool OnMouseButtonReleased(const Event& event);

  private:

    void OnAttack(const GameEvent& event);
    void OnHeal(const GameEvent& event);
    void OnDeath(const GameEvent& event);

    bool ValidMovement(GameState& state, const Event& event);
    bool ValidMultiPhaseAction(GameState& state, const Event& event);
    bool ValidSinglePhaseAction(GameState& state, const Event& event);

    void NextPlayer(GameState& state);

    typedef function<bool(GameState&, const Event&)> fnProcessAction;

    fnTileAtPos _fnTileAtPos;
    map<PlayerAction, SpellBase*> _actionMap;
  };
}

