#pragma once
#include "roguelike.hpp"
#include "game_state.hpp"
#include "event_manager.hpp"

namespace rogue
{
  class GamePlayer
  {
  public:
    void Update(GameState& gameState);
    bool OnKeyPressed(GameState& state, const Event& event);
  };
}
