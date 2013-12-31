#pragma once

namespace rogue
{
  class Level;
  class Party;

  class Player;
  class Monster;

  enum class PlayerAction : u8
  {
    kUnknown,

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

    Player* GetActivePlayer() const;
    size_t _elapsedTime;
    size_t _activePlayer;
    Level *_level;
    Party *_party;
    int _actionPhase;
    PlayerAction _playerAction;
    bool _monsterPhase;
  };

  void UpdateState(GameState& state, Event* event);
}
