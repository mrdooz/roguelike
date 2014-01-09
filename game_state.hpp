#pragma once
#include "utils.hpp"

namespace rogue
{
  struct SpellBase;

  class Level;
  class Party;

  class Player;
  class Monster;

  enum class PlayerAction : u8
  {
    Unknown,

    // Warrior
    MightyBlow,
    Charge,
    Cleave,
    Taunt,

    // Cleric
    Smite,
    MinorHeal,
    MajorHeal,
    Resurrect,

    // Wizard
    ArcaneBlast,
    LightningBolt,
    Fireball,
    PoisonCloud,

    // Ranger
    SingleShot,
    ImmobilizingShot,
    MultiShot,
    Leap,
  };

  enum class Selection
  {
    None      = 0,
    Empty     = 1 << 0,
    Monster   = 1 << 1,
    Player    = 1 << 2,
    Any       = (int)Empty | (int)Monster | (int)Player,
  };

  class GameState
  {
  public:
    GameState();
    ~GameState();

    string _description;
    Player* GetActivePlayer() const;
    u32 _elapsedTime;
    u32 _activePlayer;
    Level *_level;
    Party *_party;
    int _actionPhase;
    int _selection;   // Waiting for input
    int _selectionRange;
    Pos _selectionOrg;
    PlayerAction _playerAction;
    SpellBase* _curSpell;
    bool _monsterPhase;

  private:
    DISALLOW_COPY_AND_ASSIGN(GameState);
  };

  void UpdateState(GameState& state, Event* event);
}
