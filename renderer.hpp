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

    bool Init(const GameState& state);
    void DrawWorld(const GameState& state);
    void Resize(const GameState& state);

    void onMoveDone();

    void Update(const GameState& state, const sf::Event* event);
  private:

    // Convert a pos in world (global) space to window space
    Pos ToLocal(const Pos& pos) const;
    Pos ToGlobal(const Pos& pos) const;

    void DrawParty(const GameState& state);
    void drawPartyStats(const GameState& state);
    void DrawLevel(const GameState& state);
    void DrawMonsters(const GameState& state);

    void DrawHealthBar(int health, int maxHealth, const Pos &pos);

    int _partyStatsWidth;
    void VisibleArea(int* rows, int* cols) const;
    void ClampedVisibleArea(const Level* level, int* rows, int* cols) const;
    RenderWindow *_window;

    vector<sf::Sprite> _tileSprites;

    int _leftMargin, _rightMargin, _topMargin, _bottomMargin;
    Pos _offset;
    int _zoomLevel;
    Font _font;
    Texture _environmentTexture;
    Texture _characterTexture;
  };
}


#endif