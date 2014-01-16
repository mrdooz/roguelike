#pragma once
#include "game_state.hpp"
#include "game_event_manager.hpp"
#include "window_event_manager.hpp"

namespace rogue
{
  struct SpellBase;

  class ActionMap
  {
  public:
    typedef function<void(Keyboard::Key)> fnKeyboardCallback;
    typedef function<void(Entity*)> fnSelectionCallback;

    typedef int SelectionMask;
    typedef map<pair<Keyboard::Key, SelectionMask>, fnSelectionCallback> SelectionCallback;
    SelectionCallback::iterator _selectionCandidate;
    SelectionCallback _selectionCallbacks;
    map<Keyboard::Key, fnKeyboardCallback> _keyboardCallbacks;
  };

  class GamePlayer
  {
  public:
    typedef function<int (const GameState&, int, int)> fnTileAtPos;

    GamePlayer(WindowEventManager* windowEventManager, const fnTileAtPos& fnTileAtPos);
    ~GamePlayer();

    bool Init();

    void Update(GameState& gameState);
    bool OnKeyReleased(const Event& event);
    bool OnMouseButtonReleased(const Event& event);

  private:
    void InitActionMaps();
    void OnMovement(Keyboard::Key key);
    void OnLightningBolt(Entity* entity);

    void OnAttack(const GameEvent& event);
    void OnHeal(const GameEvent& event);
    void OnDeath(const GameEvent& event);
    void OnLevelGained(const GameEvent& event);
    void OnItemGained(const GameEvent& event);

    bool ValidMovement(GameState& state, const Event& event);
    bool ValidMultiPhaseAction(GameState& state, const Event& event);
    bool ValidSinglePhaseAction(GameState& state, const Event& event);

    void NextPlayer();

    typedef function<bool(GameState&, const Event&)> fnProcessAction;

    ActionMap _actionMapWizard;
    ActionMap _actionMapRanger;
    ActionMap _actionMapWarrior;
    ActionMap _actionMapCleric;

    ActionMap* _curActionMap;

    WindowEventManager* _windowEventManager;
    fnTileAtPos _fnTileAtPos;
    map<PlayerAction, SpellBase*> _actionMap;

    bool _advancePlayer;
  };
}

