#pragma once

namespace rogue
{
  class GameState;
  class Monster;
  class Player;

  struct SpellBase
  {
    virtual ~SpellBase() {}
    virtual bool IsValid(GameState& state, const Event& event) = 0;
    virtual bool OnMonsterSelected(GameState& state, Monster* monster);
    virtual bool OnPlayerSelected(GameState& state, Player* player);
    virtual bool Finished(const GameState& state);
  };

  struct SpellCharge : public SpellBase
  {
    virtual bool IsValid(GameState& state, const Event& event);
  };

  struct SpellMightyBlow : public SpellBase
  {
    virtual bool IsValid(GameState& state, const Event& event);
  };

  struct SpellArcaneBlast : public SpellBase
  {
    virtual bool IsValid(GameState& state, const Event& event);
    virtual bool OnMonsterSelected(GameState& state, Monster* monster);
    virtual bool Finished(const GameState& state);
  };

  struct SpellLightningBolt : public SpellBase
  {
    virtual bool IsValid(GameState& state, const Event& event);
  };

  struct SpellFireball : public SpellBase
  {
    virtual bool IsValid(GameState& state, const Event& event);
  };

  struct SpellPoisonCloud : public SpellBase
  {
    virtual bool IsValid(GameState& state, const Event& event);
  };
}

