#include "monster.hpp"
#include "utils.hpp"
#include "player.hpp"

using namespace rogue;

Monster::Monster()
    : Entity(Type::Monster)
    , _aggroPlayer(nullptr)
    , _tauntPlayer(nullptr)
    , _tauntRemaining(0)
    , _stunRemaining(0)
    , _immobilizeRemaining(0)
    , _action(Action::kUnknown)
    , _visibilityRange(5)
    , _aggroDecay(0)
    , _playerVisible(false)
    , _attackRange(0)
{
}

void Monster::DebugDump(vector<string>& dump)
{
  dump.push_back(toString("Aggro: 0x%.8x", _aggroPlayer));
  dump.push_back(toString("AggroDecay: %d", _aggroDecay));
  dump.push_back(toString("PlayerVisible: %s", _playerVisible ? "Y" : "N"));
}

void Monster::DebugDraw(RenderTarget& rt)
{
  if (_aggroPlayer)
  {
    int s = 3 * 8;
    int o = 3 * 4;
    Color col = _playerVisible ? Color::White : Color::Red;
    sf::Vertex verts[] = {
      MakeVertex(s * _pos.x + o, s * _pos.y + o, col),
      MakeVertex(s * _aggroPlayer->GetPos().x + o, s * _aggroPlayer->GetPos().y + o, col)
    };

    rt.draw(verts, 2, sf::Lines);
  }
}
