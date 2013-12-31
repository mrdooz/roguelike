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
GamePlayer::GamePlayer()
{
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
  if (ArrowKeyToOffset(key, &ofs))
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
  }
  keys[] =
  {
    { PlayerClass::kWarrior, Keyboard::Num0, Keyboard::M, PlayerAction::MightyBlow },
    { PlayerClass::kWarrior, Keyboard::Num1, Keyboard::C, PlayerAction::Charge },
    { PlayerClass::kWarrior, Keyboard::Num2, Keyboard::L, PlayerAction::Cleave },
    { PlayerClass::kWarrior, Keyboard::Num3, Keyboard::T, PlayerAction::Taunt },

    { PlayerClass::kCleric, Keyboard::Num0, Keyboard::S, PlayerAction::Smite },
    { PlayerClass::kCleric, Keyboard::Num1, Keyboard::M, PlayerAction::MinorHeal },
    { PlayerClass::kCleric, Keyboard::Num2, Keyboard::A, PlayerAction::MajorHeal },
    { PlayerClass::kCleric, Keyboard::Num3, Keyboard::R, PlayerAction::Resurrect },

    { PlayerClass::kWizard, Keyboard::Num0, Keyboard::A, PlayerAction::ArcaneBlast },
    { PlayerClass::kWizard, Keyboard::Num1, Keyboard::L, PlayerAction::LightningBolt },
    { PlayerClass::kWizard, Keyboard::Num2, Keyboard::F, PlayerAction::Fireball },
    { PlayerClass::kWizard, Keyboard::Num3, Keyboard::P, PlayerAction::PoisonCloud },

    { PlayerClass::kRanger, Keyboard::Num0, Keyboard::S, PlayerAction::SingleShot },
    { PlayerClass::kRanger, Keyboard::Num1, Keyboard::I, PlayerAction::ImmobilizingShot },
    { PlayerClass::kRanger, Keyboard::Num2, Keyboard::M, PlayerAction::MultiShot },
  };

  for (size_t i = 0; i < ELEMS_IN_ARRAY(keys); ++i)
  {
    auto& cur = keys[i];
    if (player->_class == cur._class && (key == cur._key || key == cur._altKey))
    {
      state._playerAction = cur._action;
      return true;
    }
  }
  return false;
}

//-----------------------------------------------------------------------------
bool GamePlayer::ProcessCharge(GameState& state, const Event& event)
{
  auto party = state._party;
  auto player = party->_players[state._activePlayer];
  auto level = state._level;

  // for a charge to be valid, there must be a free path to a mob in the given
  // direction
  Monster* monster = nullptr;
  Pos ofs;
  if (ArrowKeyToOffset(event.key.code, &ofs))
  {
    for (size_t i = 0; i < player->_chargeRange; ++i)
    {
      Pos newPos(player->_pos + (int)i * ofs);
      auto& tile = level->Get(newPos);
      if (tile._type != TileType::kFloor || tile._player)
        break;

      if (tile._type == TileType::kFloor && tile._monster)
      {
        monster = tile._monster;
        break;
      }
    }
  }

  if (monster)
  {
    monster->_stunRemaining += 3;
  }

  return !!monster;
}

//-----------------------------------------------------------------------------
bool GamePlayer::ProcessMightyBlow(GameState& state, const Event& event)
{
  Pos ofs;
  if (ArrowKeyToOffset(event.key.code, &ofs))
  {
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

