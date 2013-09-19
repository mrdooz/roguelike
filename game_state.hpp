#ifndef _GAME_STATE_HPP_
#define _GAME_STATE_HPP_

#include "types.hpp"

namespace rogue
{
  class Level;
  class Party;

  class Player;
  class Monster;

  enum class GameState {
    kPlayerState,
    kAiState,
  };

  typedef boost::coroutines::coroutine<int(sf::Keyboard::Key)> UpdateCoro;

  struct StateBase {

    virtual GameState stateId() const = 0;
    virtual void enterState() {}
    virtual void leaveState() {}
    virtual GameState update() = 0;
    virtual GameState handleEvent(const sf::Event &event) = 0;

    void DoFunkyStuff(UpdateCoro::caller_type& coro);
    void handleAttack(Player *player, Monster *monster);

    Level *_level;
    Party *_party;
  };

  struct PlayerState : public StateBase {

    virtual GameState stateId() const OVERRIDE { return GameState::kPlayerState; }
    virtual void enterState() OVERRIDE;
    virtual void leaveState() OVERRIDE;
    virtual GameState update() OVERRIDE { return GameState::kPlayerState; }
    virtual GameState handleEvent(const sf::Event &event) OVERRIDE;
    //void handleAttack(Player *player, Monster *monster);

    typedef function<void()> fnDoneListener;
    void addMoveDoneListener(const fnDoneListener &fn);

    vector<fnDoneListener> _listeners;
  };

  struct AiState : public StateBase {
    virtual void enterState() OVERRIDE;
    virtual void leaveState() OVERRIDE;
    virtual GameState stateId() const OVERRIDE { return GameState::kAiState; }
    virtual GameState update() OVERRIDE;
    virtual GameState handleEvent(const sf::Event &event) OVERRIDE { return GameState::kAiState; }
  };

}

#endif
