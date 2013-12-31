#pragma once

namespace rogue
{
  struct SpellBase
  {
    virtual ~SpellBase() {}
    virtual bool IsValid(GameState& state, const Event& event) = 0;
  };

  struct SpellCharge : public SpellBase
  {
    virtual bool IsValid(GameState& state, const Event& event);
  };

  struct SpellMightyBlow : public SpellBase
  {
    virtual bool IsValid(GameState& state, const Event& event);
  };

  struct SpellLightningBolt : public SpellBase
  {
    virtual bool IsValid(GameState& state, const Event& event);
  };

  struct SpellFireball : public SpellBase
  {
    virtual bool IsValid(GameState& state, const Event& event);
  };

  struct SpellArcaneBlast : public SpellBase
  {
    virtual bool IsValid(GameState& state, const Event& event);
  };

  struct SpellPoisonCloud : public SpellBase
  {
    virtual bool IsValid(GameState& state, const Event& event);
  };
}

