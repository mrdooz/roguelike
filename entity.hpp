#pragma once

namespace rogue
{
  class Entity
  {
    friend class PlayerFactory;
    friend class LevelFactory;
    
    public:

    virtual bool IsHuman() const = 0;

    int Level() const { return _level; }
    void SetLevel(int level) { _level = level; }
    int Armor() const { return _armor; }
    int ArmorBonus() const { return _armorBonus; }
    void SetArmorBonus(int bonus);
    int WeaponBonus() const { return _weaponBonus; }
    void SetWeaponBonus(int bonus);
    int CurHealth() const { return _curHealth; }
    void SetHealth(int health); 
    int MaxHealth() const { return _maxHealth; }
    int CurMana() const { return _curMana; }
    int MaxMana() const { return _maxMana; }

    Pos& GetPos() { return _pos; }
    void SetPos(const Pos& pos) { _pos = pos ;}
    const string& Name() const { return _name; }

    virtual void DebugDump(vector<string>& dump) {}
    virtual void DebugDraw(RenderTarget& rt) {}

  protected:
    Entity();
    virtual ~Entity() {}
    
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
  };
}
