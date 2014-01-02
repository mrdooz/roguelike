#pragma once

#include "level.hpp"

namespace rogue
{
  class LevelFactory
  {
  public:
    static Level *CreateLevel(int width, int height);

  private:
    static void CreateMonsters(Level* level);
  };
}
