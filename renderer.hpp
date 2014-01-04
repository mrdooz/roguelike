#pragma once

#include "game_event_manager.hpp"
#include "level.hpp"
#include "animated_sprite.hpp"
#include "texture_cache.hpp"

namespace rogue
{
  class Level;
  class Party;
  class GameState;

  class Renderer
  {
  public:
    Renderer(RenderWindow *window);
    ~Renderer();

    bool Init(const GameState& state);
    void DrawWorld(const GameState& state);
    void Resize(const GameState& state);

    void Update(const GameState& state, const sf::Event* event);

    void OnMouseMove(const GameState& state, int x, int y, bool hover);

    // Given the x/y window coordinates, return the tile at that position
    int TileAtPos(const GameState& state, int x, int y) const;

  private:
    bool OnMouseButtonReleased(const Event& event);
    bool OnKeyPressed(const Event& event);

    void OnAttack(const GameEvent& event);
    void OnHeal(const GameEvent& event);
    void OnDeath(const GameEvent& event);
    void OnItemGained(const GameEvent& event);
    void OnLevelGained(const GameEvent& event);

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
    void DrawAnimations();

    void DrawHealthBar(int health, int maxHealth, const Pos &pos);

    void VisibleArea(const Level* level, int* rows, int* cols) const;

    void AddToCombatLog(const string& msg);
    void DrawCombatLog();

    void AddAnimation(
        Animation::Id id,
        const Pos& startPos,
        const Pos& endPos,
        const time_duration& duration);
    bool InitAnimations();

    Pos HalfOffset() const;

    RenderWindow *_window;
    int _partyStatsWidth;
    int _prevSelected;

    vector<string> _combatLog;

    map<LootItem::Type, Rect> _objectToTextureRect;

    vector<Sprite> _tileSprites;
    Pos _offset;
    int _leftMargin, _rightMargin, _topMargin, _bottomMargin;
    int _zoomLevel;
    TextureCache _textureCache;

    Font* _font;
    Texture* _environmentTexture;
    Texture* _characterTexture;
    Texture* _objectTexture;

    Sprite _objectSprite;
    Sprite _sprMain;
    Sprite _sprCharacter;
    Sprite _sprCombatLog;
    RenderTexture _rtMain;
    RenderTexture _rtCharacterPane;
    RenderTexture _rtCombatLog;

    map<Animation::Id, Animation*> _animationMap;
    deque<AnimationInstance> _activeAnimations;

    Entity* _debugDump;
  };
}
