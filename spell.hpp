#pragma once

#include "animation.hpp"

namespace rogue
{
  class GameState;
  class Monster;
  class Player;

  struct SpellBase
  {
    virtual ~SpellBase() {}
    virtual bool IsValid(GameState& state, const Event& event);
    virtual bool OnMonsterSelected(GameState& state, Monster* monster);
    virtual bool OnPlayerSelected(GameState& state, Player* player);
    virtual bool Finished(const GameState& state);
    virtual Animation::Id AnimationId();

  };

  // Wizard spells
  struct SpellArcaneBlast : public SpellBase
  {
    virtual bool OnMonsterSelected(GameState& state, Monster* monster);
    virtual bool Finished(const GameState& state);
    virtual Animation::Id AnimationId();
  };

  struct SpellLightningBolt : public SpellBase
  {
    virtual bool OnMonsterSelected(GameState& state, Monster* monster);
    virtual Animation::Id AnimationId();
  };

  struct SpellFireball : public SpellBase
  {
  };

  struct SpellPoisonCloud : public SpellBase
  {
  };


  // Warrior spells
  struct SpellCharge : public SpellBase
  {
    virtual bool IsValid(GameState& state, const Event& event);
  };

  struct SpellMightyBlow : public SpellBase
  {
  };
}

