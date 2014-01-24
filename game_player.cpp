#include "game_player.hpp"
#include "party.hpp"
#include "level.hpp"
#include "player.hpp"
#include "monster.hpp"
#include "utils.hpp"
#include "game.hpp"
#include "spell.hpp"
#include "window_event_manager.hpp"
#include "renderer.hpp"

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
    Renderer* renderer)
  : _windowEventManager(windowEventManager)
  , _renderer(renderer)
  , _advancePlayer(false)
  , _selectionCandidate(nullptr)
  , _curActionMap(nullptr)
{
}

//-----------------------------------------------------------------------------
GamePlayer::~GamePlayer()
{
}

//-----------------------------------------------------------------------------
bool GamePlayer::Init()
{
  _windowEventManager->RegisterHandler(Event::KeyReleased, bind(&GamePlayer::OnKeyReleased, this, _1));

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

  Pos a = player->GetPos();
  Pos b = monster->GetPos();
  // Lightning can only shoot in a straight line
  if (a.x != b.x && a.y != b.y)
  {
    GAME->AddPlayerMessage("Lightning only shoots straight");
    return;
  }

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
    //event._spell = _spellMap[PlayerAction::LightningBolt];

    event._damage = (5 * player->Level() + player->WeaponBonus());

    GAME_EVENT->SendEvent(event);
  }

  //_renderer->AddAnimation()

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
    return true;
  }
  else
  {
    // look for selection actions
    auto it = _curActionMap->_selectionCallbacks.find(key);
    if (it != _curActionMap->_selectionCallbacks.end())
    {
      _selectionCandidate = &it->second;
      return true;
    }
  }

  return false;
}
