#ifndef _RENDERER_HPP_
#define _RENDERER_HPP_

#include "types.hpp"

namespace rogue
{
  class Level;
  class Party;

  class Renderer {
  public:
    Renderer(RenderWindow *window);

    bool init(Level *level, Party *party);
    void drawWorld();

    void onMoveDone();
  private:

    void drawParty();
    void drawPartyStats();
    void drawLevel();
    void drawMonsters();

    void getVisibleArea(Pos *topLeft, int *rows, int *cols);

    void drawHealthBar(int health, int maxHealth, const Pos &pos);

    int _partyStatsWidth;
    RenderWindow *_window;

    int _zoomLevel;
    Font _font;
    Texture _environmentTexture;
    Texture _characterTexture;

    Level *_level;
    Party *_party;
  };
}


#endif