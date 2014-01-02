#include "game_player.hpp"
#include "party.hpp"
#include "level.hpp"
#include "player.hpp"
#include "monster.hpp"
#include "utils.hpp"
#include "game.hpp"
#include "spell.hpp"

using namespace rogue;

namespace
{
  //-----------------------------------------------------------------------------
  bool ArrowKeyToHeading(sf::Keyboard::Key code, Pos *ofs, Heading* heading)
  {
    switch (code)
    {
      case sf::Keyboard::Left:  *ofs = Pos(-1,0); *heading = Heading::West; return true;
      case sf::Keyboard::Right: *ofs = Pos(+1,0); *heading = Heading::East; return true;
      case sf::Keyboard::Up:    *ofs = Pos(0,-1); *heading = Heading::North; return true;
      case sf::Keyboard::Down:  *ofs = Pos(0,+1); *heading = Heading::South; return true;
    }
    return false;
  }

#if 0
  static bool isTwoPhaseAction(PlayerAction action)
  {
    return action == PlayerAction::kMeleeAttack
    || action == PlayerAction::kRangedAttack
    || action == PlayerAction::kSpellAttack;
  }

  bool keyToTwoPhase(sf::Keyboard::Key code, PlayerAction *action)
  {
    switch (code)
    {
      case sf::Keyboard::A: *action = PlayerAction::kMeleeAttack; return true;
      case sf::Keyboard::R: *action = PlayerAction::kRangedAttack; return true;
      case sf::Keyboard::S: *action = PlayerAction::kSpellAttack; return true;
    }
    return false;
  }
#endif
  //-----------------------------------------------------------------------------
  void HandleAttack(GameState& state, Player *player, Monster *monster)
  {
    if (0 == --monster->_health)
    {
      state._level->monsterKilled(monster);
//      GAME.addLogMessage("Player %s killed monster!\n", player->_name.c_str());
    }
  }

}

//-----------------------------------------------------------------------------
GamePlayer::GamePlayer(const fnTileAtPos& fnTileAtPos)
  : _fnTileAtPos(fnTileAtPos)
{
  _actionMap[PlayerAction::ArcaneBlast] = new SpellArcaneBlast();

  _actionMap[PlayerAction::MightyBlow] = new SpellMightyBlow();
  _actionMap[PlayerAction::Charge] = new SpellCharge();
}

//-----------------------------------------------------------------------------
GamePlayer::~GamePlayer()
{
  for (auto a : _actionMap)
  {
    delete a.second;
  }
  _actionMap.clear();
}

//-----------------------------------------------------------------------------
void GamePlayer::Update(GameState& gameState)
{
}

//-----------------------------------------------------------------------------
bool GamePlayer::ValidMovement(GameState& state, const Event& event)
{
  auto party = state._party;
  auto player = party->_players[state._activePlayer];
  Keyboard::Key key = event.key.code;

  // Check if the input is a valid movement key, and the
  // resulting position is valid
  Pos ofs;
  if (ArrowKeyToHeading(key, &ofs, &player->_heading))
  {
    Pos newPos(player->_pos + ofs);
    if (state._level->validDestination(newPos))
    {
      player->_hasMoved = true;
      state._level->movePlayer(player, player->_pos, newPos);
      player->_pos = newPos;
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
    { PlayerClass::kWizard, Keyboard::Num1, Keyboard::L, PlayerAction::LightningBolt,     Selection::None,      "LightningBolt" },
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
      state._curSpell = _actionMap[cur._action];
      state._playerAction = cur._action;
      state._description = cur._desc;
      if (cur._selection != Selection::None)
      {
        state._selection = (int)cur._selection;
        state._selectionRange = 5;
        state._selectionOrg = player->_pos; 
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

float Dist(const Pos& a, const Pos& b)
{
  float dx = a.x - b.x;
  float dy = a.y - b.y;
  return sqrtf(dx*dx+dy*dy);
}

//-----------------------------------------------------------------------------
bool GamePlayer::OnMouseButtonReleased(GameState& state, const Event& event)
{
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
    Monster* monster = tile._monster;
    if (Dist(monster->_pos, state._selectionOrg) <= state._selectionRange)
    {
      state._curSpell->OnMonsterSelected(state, monster);
    }
  }
  else if (tile._player && (flags & (int)Selection::Player))
  {
    Player* player = tile._player;
    if (Dist(player->_pos, state._selectionOrg) <= state._selectionRange)
    {
      state._curSpell->OnPlayerSelected(state, player);
    }

    state._curSpell->OnPlayerSelected(state, tile._player);
  }
  else
  {
    // check for empty tiles..
  }

  return true;
}

//-----------------------------------------------------------------------------
bool GamePlayer::OnKeyPressed(GameState& state, const Event& event)
{
  if (state._monsterPhase)
    return false;

  size_t activePlayer = state._activePlayer;
  auto level = state._level;
  auto party = state._party;

  bool validAction = false;
  bool nextPlayer = false;

  Keyboard::Key key = event.key.code;

  // Tick the active player
  auto player = party->_players[state._activePlayer];

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

    // check for next phase of a multi phase action
    if (state._actionPhase > 0)
    {
      // Find the current spell validator
      auto it = _actionMap.find(state._playerAction);
      if (it != _actionMap.end())
      {
        auto& spell = it->second;
        if (spell->IsValid(state, event))
        {

        }
      }
      // for the current action, determine if the current key is legal

      Pos ofs;
      if (ArrowKeyToOffset(key, &ofs))
      {
        if (Monster *monster = level->monsterAt(player->_pos + ofs))
        {
          HandleAttack(state, player, monster);
          player->_hasMoved = true;
          validAction = true;
          break;
        }
      }
    }
  }
  while (false);

  if (validAction)
  {
    if (nextPlayer)
    {
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
  }

  return true;
}

