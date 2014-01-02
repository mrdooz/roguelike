#pragma once

namespace rogue
{
  class Entity
  {
    friend class PlayerFactory;
    friend class LevelFactory;
    
    public:

    enum class Type
    {
      Player,
      Monster,
    };

    Type GetType() const { return _type; }
    int Level() const { return _level; }
    int WeaponBonus() const { return _weaponBonus; }
    int CurHealth() const { return _curHealth; }
    void SetHealth(int health); 
    int MaxHealth() const { return _maxHealth; }
    int CurMana() const { return _curMana; }
    int MaxMana() const { return _maxMana; }

    Pos& GetPos() { return _pos; }
    void SetPos(const Pos& pos) { _pos = pos ;}
    const string& Name() const { return _name; }

  protected:
    Entity(Type type);
    
    Pos _pos;
    string _name;

    int _armor;

    int _armorBonus;
    int _weaponBonus;

    int _curHealth;
    int _maxHealth;
    int _curMana;
    int _maxMana;

    int _level;

  private:
    Type _type;
  };
}