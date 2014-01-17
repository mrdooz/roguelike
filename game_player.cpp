#include "game_player.hpp"
#include "party.hpp"
#include "level.hpp"
#include "player.hpp"
#include "monster.hpp"
#include "utils.hpp"
#include "game.hpp"
#include "spell.hpp"
#include "window_event_manager.hpp"

using namespace rogue;

namespace
{
  //-----------------------------------------------------------------------------
  bool ArrowKeyToHeading(Keyboard::Key code, Pos *ofs, Heading* heading)
  {
    switch (code)
    {
      case Keyboard::Left:  *ofs = Pos(-1,0); *heading = Heading::West; return true;
      case Keyboard::Right: *ofs = Pos(+1,0); *heading = Heading::East; return true;
      case Keyboard::Up:    *ofs = Pos(0,-1); *heading = Heading::North; return true;
      case Keyboard::Down:  *ofs = Pos(0,+1); *heading = Heading::South; return true;
    }
    return false;
  }
}

//-----------------------------------------------------------------------------
GamePlayer::GamePlayer(
    WindowEventManager* windowEventManager,
    const fnTileAtPos& fnTileAtPos)
  : _windowEventManager(windowEventManager)
  , _fnTileAtPos(fnTileAtPos)
  , _advancePlayer(false)
  , _selectionCandidate(nullptr)
  , _curActionMap(nullptr)
{
  _spellMap[PlayerAction::ArcaneBlast] = new SpellArcaneBlast();
  _spellMap[PlayerAction::LightningBolt] = new SpellLightningBolt();

  _spellMap[PlayerAction::MightyBlow] = new SpellMightyBlow();
  _spellMap[PlayerAction::Charge] = new SpellCharge();
}

//-----------------------------------------------------------------------------
GamePlayer::~GamePlayer()
{
  AssocDelete(&_spellMap);
}

//-----------------------------------------------------------------------------
bool GamePlayer::Init()
{
  _windowEventManager->RegisterHandler(Event::KeyReleased, bind(&GamePlayer::OnKeyReleased, this, _1));
//  _windowEventManager->RegisterHandler(Event::MouseButtonReleased, bind(&GamePlayer::OnMouseButtonReleased, this, _1));

  GAME_EVENT->RegisterHandler(GameEvent::Type::Attack, bind(&GamePlayer::OnAttack, this, _1));
  GAME_EVENT->RegisterHandler(GameEvent::Type::Heal, bind(&GamePlayer::OnHeal, this, _1));
  GAME_EVENT->RegisterHandler(GameEvent::Type::Death, bind(&GamePlayer::OnDeath, this, _1));
  GAME_EVENT->RegisterHandler(GameEvent::Type::LevelGained, bind(&GamePlayer::OnLevelGained, this, _1));
  GAME_EVENT->RegisterHandler(GameEvent::Type::ItemGained, bind(&GamePlayer::OnItemGained, this, _1));

  GAME->RegisterSelectionListener(bind(&GamePlayer::OnSelectionEvent, this, _1));

  InitActionMaps();

  return true;
}

//-----------------------------------------------------------------------------
void GamePlayer::InitActionMaps()
{
  _actionMapWizard._keyboardCallbacks[Keyboard::Up]     = bind(&GamePlayer::OnMovement, this, _1);
  _actionMapWizard._keyboardCallbacks[Keyboard::Down]   = bind(&GamePlayer::OnMovement, this, _1);
  _actionMapWizard._keyboardCallbacks[Keyboard::Left]   = bind(&GamePlayer::OnMovement, this, _1);
  _actionMapWizard._keyboardCallbacks[Keyboard::Right]  = bind(&GamePlayer::OnMovement, this, _1);

  _actionMapWizard._selectionCallbacks[Keyboard::L] =
    make_pair((int)Selection::Monster, bind(&GamePlayer::OnLightningBolt, this, _1));

}

//-----------------------------------------------------------------------------
void GamePlayer::OnLightningBolt(const SelectionEvent& event)
{
  // Check for LOS to mob
  const GameState& state = GAME->GetGameState();
  Player* player = state.GetActivePlayer();
  MonsterPtr monster = static_pointer_cast<Monster>(event._entity);
  int dist = state._level->IsVisible(player->GetPos(), monster->GetPos());
  if (!dist)
  {
    GAME->AddPlayerMessage("Target is not visible");
    return;
  }

  if (dist > 4)
  {
    GAME->AddPlayerMessage("Target out of range");
    return;
  }

  // Zap all mobs in the LOS (skip self!)
  vector<Entity*> mobs;
  state._level->EntitiesInPath(player->GetPos(), monster->GetPos(), &mobs);

  for (size_t i = 1; i < mobs.size(); ++i)
  {
    GameEvent event(GameEvent::Type::Attack);
    event._agent = player;
    event._target = mobs[i];
    event._spell = _spellMap[PlayerAction::LightningBolt];

    event._damage = (5 * player->Level() + player->WeaponBonus());

    GAME_EVENT->SendEvent(event);
  }
  NextPlayer();
}

//-----------------------------------------------------------------------------
void GamePlayer::OnMovement(Keyboard::Key key)
{
  const GameState& state = GAME->GetGameState();
  Party* party = state._party;
  Level* level = state._level;
  Player* player = party->_players[state._activePlayer].get();

  // Check if the input is a valid movement key, and the
  // resulting position is valid
  Pos ofs;
  if (!ArrowKeyToHeading(key, &ofs, &player->_heading))
    return;

  Pos newPos(player->GetPos() + ofs);
  if (!level->ValidDestination(newPos))
  {
    GAME->AddPlayerMessage("You can't move there");
    return;
  }

  level->MovePlayer(player, newPos);

  // If the new tile has any items, pick up the top one
  Tile& tile = level->Get(newPos);
  if (!tile._items.empty())
  {
    GameEvent event(player, player, new LootItem(tile._items.back()));
    tile._items.pop_back();
    GAME_EVENT->SendEvent(event);
  }

  NextPlayer();
}

//-----------------------------------------------------------------------------
void GamePlayer::Update(GameState& gameState)
{
  // Check if it's time to set a new action map
  if (gameState._monsterPhase || gameState._playerInProgress)
    return;

  const Player* player = gameState.GetActivePlayer();
  gameState._playerInProgress = true;

  switch (player->_class)
  {
  case PlayerClass::kWizard:
    _curActionMap = &_actionMapWizard;
    break;

  case PlayerClass::kRanger:
    _curActionMap = &_actionMapRanger;
    break;

  case PlayerClass::kWarrior:
    _curActionMap = &_actionMapWarrior;
    break;

  case PlayerClass::kCleric:
    _curActionMap = &_actionMapCleric;
    break;
  }

}

//-----------------------------------------------------------------------------
void GamePlayer::OnAttack(const GameEvent& event)
{
  auto target = event._target;
  target->SetHealth(target->CurHealth() - event._damage);

  // Check if the attack is fatal
  if (target->CurHealth() == 0)
  {
    GameEvent deathEvent(GameEvent::Type::Death);
    deathEvent._agent = event._agent;
    deathEvent._target = event._target;
    GAME_EVENT->SendEvent(deathEvent);
  }
}

//-----------------------------------------------------------------------------
void GamePlayer::OnHeal(const GameEvent& event)
{

}

//-----------------------------------------------------------------------------
void GamePlayer::OnDeath(const GameEvent& event)
{
  // If a monster is killed, calc xp, and give it to all the players
  EntityPtr target = event._target;
  if (!target->IsHuman())
  {
    auto& state = GAME->GetGameState();
    int xp = target->Level();
    for (auto p : state._party->_players)
    {
      if (p->CurHealth() > 0)
      {
        p->_xp += xp;
        if (p->_xp >= p->_xpForNextLevel)
        {
          GameEvent levelEvent(GameEvent::Type::LevelGained);
          levelEvent._agent = p;
          levelEvent._target = p;
          GAME_EVENT->SendEvent(levelEvent);
        }
      }
    }
  }
}

//-----------------------------------------------------------------------------
void GamePlayer::OnLevelGained(const GameEvent& event)
{
  Player* player = static_cast<Player*>(event._agent.get());
  player->SetLevel(player->Level()+1);
  player->_xp = player->_xpForNextLevel;
  player->_xpForNextLevel *= 2;
}

//-----------------------------------------------------------------------------
void GamePlayer::OnItemGained(const GameEvent& event)
{
  Player* agent = static_cast<Player*>(event._agent.get());
  const LootItem* item = event._item;
  switch (item->_type)
  {
    case LootItem::Type::Gold: agent->_gold += item->_numGold; break;
    case LootItem::Type::ManaPotion: agent->_manaPotions += item->_numPots; break;
    case LootItem::Type::HealthPotion: agent->_healthPotions += item->_numPots; break;
    case LootItem::Type::ArmorUpgrade: agent->SetArmorBonus(agent->ArmorBonus() + item->_numUpgrades); break;
    case LootItem::Type::WeaponUpgrade: agent->SetWeaponBonus(agent->WeaponBonus() + item->_numUpgrades); break;
  }
}

//-----------------------------------------------------------------------------
bool GamePlayer::ValidMovement(GameState& state, const Event& event)
{
  auto party = state._party;
  auto level = state._level;
  auto player = party->_players[state._activePlayer];
  Keyboard::Key key = event.key.code;

  // Check if the input is a valid movement key, and the
  // resulting position is valid
  Pos ofs;
  if (ArrowKeyToHeading(key, &ofs, &player->_heading))
  {
    Pos newPos(player->GetPos() + ofs);
    if (level->ValidDestination(newPos))
    {
      level->MovePlayer(player.get(), newPos);
      player->_hasMoved = true;
      player->SetPos(newPos);

      // If the new tile has any items, pick up the top one
      auto& tile = level->Get(newPos);
      if (!tile._items.empty())
      {
        GameEvent event(GameEvent::Type::ItemGained);
        event._agent = player;
        event._target = player;
        event._item = new LootItem(tile._items.back());
        tile._items.pop_back();
        GAME_EVENT->SendEvent(event);
      }

      return true;
    }
  }
  return false;
}

//-----------------------------------------------------------------------------
bool GamePlayer::ValidSinglePhaseAction(GameState& state, const Event& event)
{
  // guard, stealth, detect, intimidate

  return false;
}

//-----------------------------------------------------------------------------
bool GamePlayer::ValidMultiPhaseAction(GameState& state, const Event& event)
{
  // Check if the given key is a valid initial action for the current player

  auto party = state._party;
  auto player = party->_players[state._activePlayer];
  Keyboard::Key key = event.key.code;

  struct
  {
    PlayerClass _class;
    Keyboard::Key _key;
    Keyboard::Key _altKey;
    PlayerAction _action;
    Selection _selection;
    const char* _desc;
  }
  keys[] =
  {
    { PlayerClass::kWarrior, Keyboard::Num0, Keyboard::M, PlayerAction::MightyBlow,       Selection::Monster,   "MightyBlow" },
    { PlayerClass::kWarrior, Keyboard::Num1, Keyboard::C, PlayerAction::Charge,           Selection::None,      "Charge" },
    { PlayerClass::kWarrior, Keyboard::Num2, Keyboard::L, PlayerAction::Cleave,           Selection::None,      "Cleave" },
    { PlayerClass::kWarrior, Keyboard::Num3, Keyboard::T, PlayerAction::Taunt,            Selection::None,      "Taunt" },

    { PlayerClass::kCleric, Keyboard::Num0, Keyboard::S, PlayerAction::Smite,             Selection::Monster,   "Smite" },
    { PlayerClass::kCleric, Keyboard::Num1, Keyboard::M, PlayerAction::MinorHeal,         Selection::Player,    "MinorHeal" },
    { PlayerClass::kCleric, Keyboard::Num2, Keyboard::A, PlayerAction::MajorHeal,         Selection::Player,    "MajorHeal" },
    { PlayerClass::kCleric, Keyboard::Num3, Keyboard::R, PlayerAction::Resurrect,         Selection::Player,    "Resurrect" },

    { PlayerClass::kWizard, Keyboard::Num0, Keyboard::A, PlayerAction::ArcaneBlast,       Selection::Monster,   "ArcaneBlast" },
    { PlayerClass::kWizard, Keyboard::Num1, Keyboard::L, PlayerAction::LightningBolt,     Selection::Any,       "LightningBolt" },
    { PlayerClass::kWizard, Keyboard::Num2, Keyboard::F, PlayerAction::Fireball,          Selection::Monster,   "Fireball" },
    { PlayerClass::kWizard, Keyboard::Num3, Keyboard::P, PlayerAction::PoisonCloud,       Selection::Any,       "PoisonCloud" },

    { PlayerClass::kRanger, Keyboard::Num0, Keyboard::S, PlayerAction::SingleShot,        Selection::Monster,   "SingleShot" },
    { PlayerClass::kRanger, Keyboard::Num1, Keyboard::I, PlayerAction::ImmobilizingShot,  Selection::Monster,   "ImmobilizingShot" },
    { PlayerClass::kRanger, Keyboard::Num2, Keyboard::M, PlayerAction::MultiShot,         Selection::Monster,   "MultiShot" },
  };

  for (size_t i = 0; i < ELEMS_IN_ARRAY(keys); ++i)
  {
    auto& cur = keys[i];
    if (player->_class == cur._class && (key == cur._key || key == cur._altKey))
    {
      state._curSpell = _spellMap[cur._action];
      state._playerAction = cur._action;
      state._description = cur._desc;
      if (cur._selection != Selection::None)
      {
        state._selection = (int)cur._selection;
        state._selectionRange = 20;
        state._selectionOrg = player->GetPos();
      }
      else
      {
        state._selection = 0;
      }

      return true;
    }
  }
  return false;
}


//-----------------------------------------------------------------------------
bool GamePlayer::OnMouseButtonReleased(const Event& event)
{
  GameState& state = GAME->GetGameState();
  if (state._selection == 0)
    return false;

  // Get the tile under the cursor
  int idx = _fnTileAtPos(state, event.mouseButton.x, event.mouseButton.y);
  if (idx == -1)
    return false;

  // Check if the tile under the cursor contains the correct type of entity
  auto level = state._level;
  auto& tile = level->_tiles[idx];
  int flags = state._selection;

  if (tile._monster && (flags & (int)Selection::Monster))
  {
    MonsterPtr monster = tile._monster;
    if (Dist(monster->GetPos(), state._selectionOrg) <= state._selectionRange)
    {
      if (state._curSpell->OnMonsterSelected(state, monster.get()))
      {
        // Check if the current spell is finished, in which case update
        // the active player
        state._actionPhase++;
        if (state._curSpell->Finished(state))
        {
          NextPlayer();
        }
      }
    }
  }
  else if (tile._player && (flags & (int)Selection::Player))
  {
    PlayerPtr player = tile._player;
    if (Dist(player->GetPos(), state._selectionOrg) <= state._selectionRange)
    {
      state._curSpell->OnPlayerSelected(state, player.get());
    }

    state._curSpell->OnPlayerSelected(state, tile._player.get());
  }
  else
  {
    // check for empty tiles..
  }

  return true;
}

//-----------------------------------------------------------------------------
void GamePlayer::NextPlayer()
{
  GameState& state = GAME->GetGameState();

  if (_advancePlayer)
  {
    int activePlayer = state._activePlayer;
    activePlayer++;
    if (activePlayer == state._party->GetNumPlayers())
    {
      state._activePlayer = 0;
      state._monsterPhase = true;
    }
    else
    {
      state._activePlayer = activePlayer;
      state._actionPhase = 0;
    }
  }
  else
  {
    state._actionPhase = 0;
  }
}

//-----------------------------------------------------------------------------
void GamePlayer::OnSelectionEvent(const SelectionEvent& event)
{
  if (_selectionCandidate)
  {
    // check the selection mask
    u32 mask = _selectionCandidate->first;

    bool invokeCallback = ((mask & (u32)Selection::Empty) && !event._entity)
      || ((mask & (u32)Selection::Player) && event._entity && event._entity->IsHuman())
      || ((mask & (u32)Selection::Monster) && event._entity && !event._entity->IsHuman());

    if (invokeCallback)
    {
      _selectionCandidate->second(event);
    }
  }
}

//-----------------------------------------------------------------------------
bool GamePlayer::OnKeyReleased(const Event& event)
{
  if (!_curActionMap)
    return false;

  Keyboard::Key key = event.key.code;
  if (key == Keyboard::Escape)
  {
    _selectionCandidate = nullptr;
  }

  // look for keyboard actions
  auto it = _curActionMap->_keyboardCallbacks.find(key);
  if (it != _curActionMap->_keyboardCallbacks.end())
  {
    it->second(key);
  }
  else
  {
    // look for selection actions
    auto it = _curActionMap->_selectionCallbacks.find(key);
    if (it != _curActionMap->_selectionCallbacks.end())
    {
      _selectionCandidate = &it->second;
    }
  }

  return false;

/*

  GameState& state = GAME->GetGameState();
  if (state._monsterPhase)
    return false;

  bool validAction = false;
  bool nextPlayer = false;

  Keyboard::Key key = event.key.code;

  do
  {
    if (key == Keyboard::Escape)
    {
      // reset the action
      state._actionPhase = 0;
      state._description = "";
      validAction = true;
      break;
    }

    if (key == Keyboard::Space)
    {
      // skip the turn
      state._actionPhase = 0;
      validAction = true;
      nextPlayer = true;
      break;
    }

    if (state._actionPhase == 0)
    {
      if (ValidSinglePhaseAction(state, event))
      {
        validAction = true;
        nextPlayer = true;
        break;
      }

      if (ValidMultiPhaseAction(state, event))
      {
        state._actionPhase++;
        validAction = true;
        break;
      }

      if (ValidMovement(state, event))
      {
        validAction = true;
        nextPlayer = true;
        break;
      }
    }
  }
  while (false);

  if (validAction)
  {
    if (nextPlayer)
    {
      NextPlayer(state);
    }
  }
*/
  return true;
}
