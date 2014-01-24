#pragma once
#include "utils.hpp"
#include "selection.hpp"

namespace rogue
{
  struct SpellBase;

  class Level;
  class Party;

  class Player;
  class Monster;
  class PlayerFactory;

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

  class GameState
  {
  public:
    GameState();
    ~GameState();

    void CreateParty(PlayerFactory* playerFactory);

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
    bool _playerInProgress;

  private:
    DISALLOW_COPY_AND_ASSIGN(GameState);
  };

  void UpdateState(GameState& state, Event* event);
}
