#ifndef _RENDERER_HPP_
#define _RENDERER_HPP_

#include "types.hpp"

class Level;
class Party;

class Renderer {
public:
  Renderer(sf::RenderWindow *window);

  bool init(Level *level, Party *party);
  void drawWorld();
private:

  void drawParty();
  void drawPartyStats();
  void drawLevel();
  void drawMonsters();

  void getVisibleArea(Pos *topLeft, int *rows, int *cols);

  int _partyStatsWidth;
  sf::RenderWindow *_window;

  sf::Font _font;
  sf::Texture _environmentTexture;
  sf::Texture _characterTexture;

  Level *_level;
  Party *_party;
};

#endif