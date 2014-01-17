#pragma once
#include "game_state.hpp"
#include "game_event_manager.hpp"
#include "window_event_manager.hpp"

namespace rogue
{
  struct SpellBase;
  struct SelectionEvent;

  class ActionMap
  {
  public:
    typedef function<void(Keyboard::Key)> fnKeyboardCallback;
    typedef function<void(const SelectionEvent&)> fnSelectionCallback;
    typedef int SelectionMask;
    typedef pair<SelectionMask, fnSelectionCallback> SelectionPair;
    typedef map<Keyboard::Key, pair<SelectionMask, fnSelectionCallback> > KeyToSelectionPair;

    KeyToSelectionPair _selectionCallbacks;
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
    void OnSelectionEvent(const SelectionEvent& event);
    void InitActionMaps();
    void OnMovement(Keyboard::Key key);
    void OnLightningBolt(const SelectionEvent& event);

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

    WindowEventManager* _windowEventManager;
    fnTileAtPos _fnTileAtPos;
    map<PlayerAction, SpellBase*> _spellMap;

    bool _advancePlayer;

    ActionMap _actionMapWizard;
    ActionMap _actionMapRanger;
    ActionMap _actionMapWarrior;
    ActionMap _actionMapCleric;

    ActionMap* _curActionMap;
    ActionMap::SelectionPair* _selectionCandidate;

  };
}

