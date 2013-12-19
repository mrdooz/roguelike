#ifndef _RENDERER_HPP_
#define _RENDERER_HPP_

#include "types.hpp"
#include "roguelike.hpp"

namespace rogue
{
  class Level;
  class Party;
  struct GameState;

  class Renderer
  {
  public:
    Renderer(RenderWindow *window);

    bool init(const GameState& state);
    void drawWorld(const GameState& state);

    void onMoveDone();
  private:

    void drawParty(const GameState& state);
    void drawPartyStats(const GameState& state);
    void drawLevel(const GameState& state);
    void drawMonsters(const GameState& state);

    void drawHealthBar(int health, int maxHealth, const Pos &pos);

    int _partyStatsWidth;
    RenderWindow *_window;

    int _zoomLevel;
    Font _font;
    Texture _environmentTexture;
    Texture _characterTexture;
  };
}


#endif