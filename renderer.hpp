#pragma once

namespace rogue
{
  class Level;
  class Party;
  class GameState;

  class Renderer
  {
  public:
    Renderer(RenderWindow *window);

    bool Init(const GameState& state);
    void DrawWorld(const GameState& state);
    void Resize(const GameState& state);

    void Update(const GameState& state, const sf::Event* event);

    void OnMouseMove(const GameState& state, int x, int y, bool hover);

  private:

    void DrawQuad(const Pos& topLeft, size_t size, sf::Color color);

    // Convert a pos in world (global) space to window space
    Pos ToLocal(const Pos& pos) const;
    Pos ToGlobal(const Pos& pos) const;

    Pos PlayerToWorld(const Pos& pos) const;
    Vector2f PlayerToWorldF(const Pos& pos) const;

    void PlayerInView(const GameState& state);

    void DrawParty(const GameState& state);
    void DrawPartyStats(const GameState& state);
    void DrawLevel(const GameState& state);
    void DrawMonsters(const GameState& state);

    void DrawHealthBar(int health, int maxHealth, const Pos &pos);

    void VisibleArea(const Level* level, int* rows, int* cols) const;
    RenderWindow *_window;
    int _partyStatsWidth;
    int _prevSelected;

    vector<Sprite> _tileSprites;
    Pos _offset;
    int _leftMargin, _rightMargin, _topMargin, _bottomMargin;
    int _zoomLevel;
    Font _font;
    Texture _environmentTexture;
    Texture _characterTexture;

    Sprite _sprMain;
    Sprite _sprCharacter;
    Sprite _sprCombatLog;
    RenderTexture _rtMain;
    RenderTexture _rtCharacterPane;
    RenderTexture _rtCombatLog;
  };
}
