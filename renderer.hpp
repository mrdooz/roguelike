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
    virtual bool Init();
    int TileAtPos(const GameState& state, int x, int y) const;
  private:
    bool OnResize(const Event& event);
    bool OnKeyPressed(const Event& event);
    bool OnMouseButtonReleased(const Event& event);
    bool OnMouseMove(const Event& event);
    void VisibleArea(const Level* level, int* rows, int* cols) const;
    void DrawQuad(const Pos& topLeft, u32 size, sf::Color color);

    void DrawParty();
    void DrawLevel();
    void DrawMonsters();
    void DrawAnimations();
    void DrawHealthBar(int health, int maxHealth, const Pos &pos);

    // Convert a pos in world (global) space to window space
    Pos ToLocal(const Pos& pos) const;
    Pos ToGlobal(const Pos& pos) const;

    Pos PlayerToWorld(const Pos& pos) const;
    Vector2f PlayerToWorldF(const Pos& pos) const;

    void PlayerInView();

    Renderer* _renderer;
    int _zoomLevel;
    Pos _offset;
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
    friend class MainWindow;

  public:
    Renderer(RenderWindow* window, WindowEventManager* eventManager);
    ~Renderer();

    bool Init(const GameState& state);
    void DrawWorld(const GameState& state);

    void Update(const GameState& state, const sf::Event* event);

    // Given the x/y window coordinates, return the tile at that position
    int TileAtPos(const GameState& state, int x, int y) const;

  private:

    void OnAttack(const GameEvent& event);
    void OnHeal(const GameEvent& event);
    void OnDeath(const GameEvent& event);
    void OnItemGained(const GameEvent& event);
    void OnLevelGained(const GameEvent& event);

    void DrawHealthBar(int health, int maxHealth, const Pos &pos);

    void VisibleArea(const Level* level, int* rows, int* cols) const;

    void AddToCombatLog(const string& msg);

    void AddAnimation(
        Animation::Id id,
        const Pos& startPos,
        const Pos& endPos,
        const time_duration& duration);

    RenderWindow *_window;
    int _prevSelected;

    vector<string> _combatLog;

    map<LootItem::Type, Rect> _objectToTextureRect;

    vector<HotloadSprite> _tileSprites;

    Font _font;
    TextureHandle _environmentTexture;
    TextureHandle _characterTexture;
    TextureHandle _objectTexture;

    HotloadSprite _objectSprite;

    VirtualWindowManager _windowManager;

    deque<AnimationInstance> _activeAnimations;

    EntityPtr _debugDump;

    MainWindow* _mainWindow;
    CombatLogWindow* _combatLogWindow;
    PartyWindow* _partyWindow;

  };
}
