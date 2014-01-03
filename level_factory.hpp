#pragma once

#include "level.hpp"

namespace rogue
{
  class LevelFactory
  {
  public:
    static Level *CreateLevel(int level, int width, int height);

  private:
    static void CreateMonsters(Level* level);
    static void CreateDrops(Level* level);
    static void DebugDump(Level* level, vector<int>& roomIds, int width, int height);
  };
}
