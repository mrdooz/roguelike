#pragma once

#include "game_event_manager.hpp"
#include "level.hpp"
#include "animation.hpp"
#include "animated_sprite.hpp"
#include "texture_cache.hpp"
#include "virtual_window_manager.hpp"
#include "virtual_window.hpp"

namespace rogue
{
  class Level;
  class Party;
  class GameState;
  class WindowEventManager;
  class Renderer;

  //-----------------------------------------------------------------------------
  class MainWindow : public VirtualWindow
  {
  public:
    MainWindow(const string& title, const Vector2f& pos, const Vector2f& size, Renderer* renderer);
    virtual void Draw();

  private:
  };

  //-----------------------------------------------------------------------------
  class CombatLogWindow : public VirtualWindow
  {
  public:
    CombatLogWindow(const string& title, const Vector2f& pos, const Vector2f& size, Renderer* renderer);
    virtual void Draw();

  private:
    Renderer* _renderer;
  };

  //-----------------------------------------------------------------------------
  class PartyWindow : public VirtualWindow
  {
  public:
    PartyWindow(const string& title, const Vector2f& pos, const Vector2f& size, Renderer* renderer);
    virtual void Draw();

  private:
    Renderer* _renderer;
  };

  //-----------------------------------------------------------------------------
  class Renderer
  {
    friend class PartyWindow;
    friend class CombatLogWindow;

  public:
    Renderer(RenderWindow* window, WindowEventManager* eventManager);
    ~Renderer();

    bool Init(const GameState& state);
    void DrawWorld(const GameState& state);

    bool OnResize(const Event& event);
    bool OnMouseMove(const Event& event);

    void Update(const GameState& state, const sf::Event* event);

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

    void DrawQuad(const Pos& topLeft, u32 size, sf::Color color);

    // Convert a pos in world (global) space to window space
    Pos ToLocal(const Pos& pos) const;
    Pos ToGlobal(const Pos& pos) const;

    Pos PlayerToWorld(const Pos& pos) const;
    Vector2f PlayerToWorldF(const Pos& pos) const;

    void PlayerInView(const GameState& state);

    void DrawParty(const GameState& state);
    void DrawLevel(const GameState& state);
    void DrawMonsters(const GameState& state);
    void DrawAnimations();

    void DrawHealthBar(int health, int maxHealth, const Pos &pos);

    void VisibleArea(const Level* level, int* rows, int* cols) const;

    void AddToCombatLog(const string& msg);

    void AddAnimation(
        Animation::Id id,
        const Pos& startPos,
        const Pos& endPos,
        const time_duration& duration);

    Pos HalfOffset() const;

    RenderWindow *_window;
    int _prevSelected;

    vector<string> _combatLog;

    map<LootItem::Type, Rect> _objectToTextureRect;

    vector<HotloadSprite> _tileSprites;
    Pos _offset;
    int _zoomLevel;

    Font _font;
    TextureHandle _environmentTexture;
    TextureHandle _characterTexture;
    TextureHandle _objectTexture;

    HotloadSprite _objectSprite;

    RenderTexture* _rtMain;

    VirtualWindowManager _windowManager;

    deque<AnimationInstance> _activeAnimations;

    Entity* _debugDump;
  };
}
