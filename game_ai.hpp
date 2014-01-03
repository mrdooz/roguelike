#pragma once
#include "game_state.hpp"

namespace rogue
{
  class GameAI
  {
  public:
    void Update(GameState& state);

  private:
    void MoveMonster(Level* level, Monster* monster, const Pos& newPos);
  };
}