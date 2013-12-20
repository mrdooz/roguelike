#include "precompiled.hpp"
#include "renderer.hpp"
#include "player.hpp"
#include "utils.hpp"
#include "party.hpp"
#include "level.hpp"
#include "monster.hpp"
#include "game_state.hpp"

using namespace rogue;

//-----------------------------------------------------------------------------
enum class Tiles
{
  wallH,
  wallH_torch_anim_1,
  wAllH_torch_anim_2,
  wallH_crumbled,
  wallV,
  floorA,
  floorB,
  stairsUp,
  stairsDown,
  hole,
  trapdoorClosed,
  trapdoorOpen,
  floorC,
  floorRug,
  cNumTiles,
};

//-----------------------------------------------------------------------------
Renderer::Renderer(sf::RenderWindow *window) 
  : _window(window)
  , _partyStatsWidth(210)
  , _zoomLevel(3)
  , _leftMargin(0)
  , _rightMargin(210)
  , _topMargin(0)
  , _bottomMargin(0)
  , _leftOffset(0)
  , _topOffset(0)
{
}

//-----------------------------------------------------------------------------
void Renderer::onMoveDone()
{
  int a = 10;
}

//-----------------------------------------------------------------------------
void Renderer::DrawWorld(const GameState& state)
{
  // Because party members can be at totally differnet spots in the world, we
  // might end up having to redraw once per member.

  // Iterate all the players, and find the rectangle that displays the most players
  // Remove this one, and keep going until all players have rectangles

  const auto& players = state._party->_players;
  size_t numPlayers = players.size();
  vector<u8> playersAllocated(numPlayers);
  vector<vector<size_t> > groupedPlayers;
  size_t playersLeft = numPlayers;

  int rows, cols;
  VisibleArea(&rows, &cols);

  while (playersLeft)
  {
    vector<size_t> curGroup;

    // find the top left coordinate of the remaining players
    Pos topLeft(players[0]->_pos);
    for (size_t i = 1; i < numPlayers; ++i)
    {
      if (!playersAllocated[i])
      {
        Pos p(players[i]->_pos);
        topLeft.x = min(topLeft.x, p.x);
        topLeft.y = min(topLeft.y, p.y);
      }
    }

    Rect rect(topLeft, cols, rows);

    // Keep adding players until they no long fit in the rectangle
    for (size_t i = 0; i < numPlayers; ++i)
    {
      if (!playersAllocated[i])
      {
        Pos pos = players[i]->_pos;
        if (rect.PointInside(pos))
        {
          curGroup.push_back(i);
          playersAllocated[i] = 1;
          --playersLeft;
        }
      }
    }
    // Gone through all the players, so add the group
    groupedPlayers.push_back(curGroup);

  }

  DrawLevel(state);
  drawMonsters(state);
  drawParty(state);
  drawPartyStats(state);
}

//-----------------------------------------------------------------------------
void Renderer::VisibleArea(int* rows, int* cols) const
{
  // Determine number of tiles in the visible area (rows x cols)
  auto size = _window->getSize();
  size_t zoom = _zoomLevel * 8;
  *rows = 1 + (max(0, (int)size.y - _topMargin - _bottomMargin)) / zoom;
  *cols = 1 + (max(0, (int)size.x - _leftMargin - _rightMargin)) / zoom;
}

//-----------------------------------------------------------------------------
void Renderer::Resize()
{
  int rows, cols;
  VisibleArea(&rows, &cols);

  // Return if the new window contains the same number of tiles
  if (_tileSprites.size() == rows * cols)
    return;

  _tileSprites.resize(rows*cols);
  size_t zoom = _zoomLevel * 8;
  for (int i = 0; i < rows; ++i)
  {
    for (int j = 0; j < cols; ++j)
    {
      auto &sprite = _tileSprites[i*cols+j];
      sprite.setPosition((float)j*zoom, (float)i*zoom);
      sprite.setScale(3.0f, 3.0f);
      sprite.setTexture(_environmentTexture);
    }
  }
}

//-----------------------------------------------------------------------------
void Renderer::DrawLevel(const GameState& state)
{
  int rows, cols;
  VisibleArea(&rows, &cols);

  Level* level = state._level;

  int idx = 0;
  for (int i = 0; i < rows; ++i)
  {
    for (int j = 0; j < cols; ++j)
    {
      int r = _topOffset + i;
      int c = _leftOffset + j;
      if (!level->Inside(r, c))
        continue;

      Tile &tile = level->Get(r, c);
      sf::Sprite &sprite = _tileSprites[idx++];

      // If the tile is a wall, determine if it should be horizontal or vertical
      if (tile._type == TileType::kWall)
      {
        if (level->Inside(r+1, c) && level->Get(r+1, c)._type != TileType::kWall)
        {
          sprite.setTextureRect(sf::IntRect((int)Tiles::wallH*8, 0, 8, 8));
        }
        else
        {
          sprite.setTextureRect(sf::IntRect((int)Tiles::wallV*8, 0, 8, 8));
        }
      }
      else if (tile._type == TileType::kFloor)
      {
        sprite.setTextureRect(sf::IntRect((int)Tiles::floorC*8, 0, 8, 8));
      }
      else
      {
        assert(false);
      }

      sprite.setColor(sf::Color(tile._visited, tile._visited, tile._visited));
      _window->draw(sprite);
    }
  }
}

//-----------------------------------------------------------------------------
void Renderer::drawParty(const GameState& state)
{
  Player* activePlayer = state.GetActivePlayer();
  assert(activePlayer);
  Pos topLeft(activePlayer ? activePlayer->_pos : Pos());

  Level* level = state._level;
  Party* party = state._party;

  int rows, cols;
  VisibleArea(&rows, &cols);

  topLeft.row = max(0, topLeft.row - rows/2);
  topLeft.col = max(0, topLeft.col - cols/2);

  for (size_t i = 0; i < party->_players.size(); ++i)
  {
    Player *player = party->_players[i];
    int x = player->_pos.col - topLeft.col;
    int y = player->_pos.row - topLeft.row;
    if (x < 0 || x >= cols || y < 0 || y >= rows)
      continue;
    player->_name = toString("Player %d", i);
    player->_sprite.setPosition((float)x*_zoomLevel*8, (float)y*_zoomLevel*8);
    player->_sprite.setColor(player == activePlayer ? sf::Color(255, 255, 255) : sf::Color(127,127,127));
    _window->draw(player->_sprite);

    //drawHealthBar(player->_curHealth, player->_maxHeath, Pos(y, x));
  }
}

void Renderer::drawPartyStats(const GameState& state)
{
  Player* activePlayer = state.GetActivePlayer();
  assert(activePlayer);

  Level* level = state._level;
  Party* party = state._party;

  auto size = _window->getSize();
  float x = (float)(size.x - _partyStatsWidth);
  float col0 = x;
  float col1 = x + _partyStatsWidth/2;

  float y = 10;
  sf::Text heading("", _font, 20);
  sf::Text normal("", _font, 10);

  sf::Vector2f pos(x, y);

  auto drawHeading = [&](Player *player)
  {
    heading.setString(player->_name);
    heading.setPosition(pos);
    sf::FloatRect r = heading.getLocalBounds();
    sf::Vector2f tmpPos = pos;
    pos.x += r.width + 10;
    _window->draw(heading);
    normal.setString(toString("(%s, level %d)", playerClassToString(player->_class).c_str(), player->_level));
    pos.y += (r.height - normal.getLocalBounds().height) / 2;
    normal.setPosition(pos);
    _window->draw(normal);
    pos = tmpPos;
    pos.y += 25;
  };

  auto drawNormal = [&](const std::string &str)
  {
    normal.setString(str);
    normal.setPosition(pos);
    pos.y += 15;
    _window->draw(normal);
  };

  for (auto *player : party->_players)
  {

    heading.setColor(player == activePlayer ? sf::Color(255, 255, 255) : sf::Color(127,127,127));
    normal.setColor(player == activePlayer ? sf::Color(255, 255, 255) : sf::Color(127,127,127));

    pos.x = col0;
    drawHeading(player);
    float y = pos.y;
    drawNormal(toString("HP: %d/%d", player->_curHealth, player->_maxHeath));
    if (player->_maxMana > 0)
      drawNormal(toString("MANA: %d/%d", player->_curMana, player->_maxMana));

    pos.x = col1;
    pos.y = y;
    drawNormal(toString("STR: %d", player->_strength));
    drawNormal(toString("INT: %d", player->_intelligence));
    drawNormal(toString("DEX: %d", player->_dexterity));
    drawNormal(toString("VIT: %d", player->_vitality));

    pos.y += 10;
  }
}

void Renderer::drawHealthBar(int health, int maxHealth, const Pos &pos)
{
  float zoomF = (float)_zoomLevel;

  sf::RectangleShape rectangle;
  // deficit
  rectangle.setSize(sf::Vector2f(zoomF*6, zoomF));
  rectangle.setPosition((pos.col*8+1)*zoomF, (pos.row*8+7)*zoomF);
  rectangle.setFillColor(sf::Color(200, 10, 10));
  _window->draw(rectangle);
  // cur health
  rectangle.setSize(sf::Vector2f((float)health / maxHealth*zoomF*6, zoomF));
  rectangle.setPosition((pos.col*8+1)*zoomF, (pos.row*8+7)*zoomF);
  rectangle.setFillColor(sf::Color(10, 200, 10));
  _window->draw(rectangle);

}

void Renderer::drawMonsters(const GameState& state)
{
  Player* activePlayer = state.GetActivePlayer();
  assert(activePlayer);
  Pos topLeft(activePlayer ? activePlayer->_pos : Pos());

  Level* level = state._level;
  Party* party = state._party;

  auto size = _window->getSize();
  int zoom = 3;
  float zoomF = (float)zoom;
  int cols = (size.x - _partyStatsWidth) / (zoom*8);
  int rows = (size.y) / (zoom*8);

  topLeft.row = max(0, topLeft.row - rows/2);
  topLeft.col = max(0, topLeft.col - cols/2);

  for (auto monster : level->monsters())
  {
    if (!monster->_health)
      continue;

    int x = monster->_pos.col - topLeft.col;
    int y = monster->_pos.row - topLeft.row;
    if (x < 0 || x >= cols || y < 0 || y >= rows)
      continue;
    monster->_sprite.setPosition(zoomF*x*8, zoomF*y*8);
    _window->draw(monster->_sprite);

    // draw the roam path
    vector<sf::Vertex> path;
    for (auto& p : monster->_roamPath)
    {
      path.push_back(sf::Vertex(sf::Vector2f(p.x*8*zoomF, p.y*8*zoomF)));
    }

    if (!path.empty())
      _window->draw(path.data(), path.size(), sf::LinesStrip);

    drawHealthBar(monster->_health, monster->_maxHealth, Pos(y, x));
  }
}

//-----------------------------------------------------------------------------
bool Renderer::Init(const GameState& state)
{
  Level* level = state._level;
  Party* party = state._party;

  if (!_font.loadFromFile("gfx/wscsnrg.ttf"))
    return false;

  if (!_environmentTexture.loadFromFile("oryx_lofi/lofi_environment.png"))
    return false;

  if (!_characterTexture.loadFromFile("oryx_lofi/lofi_char.png"))
    return false;

  // Set the texture coords for the players
  for (auto p : party->_players)
  {
    p->_sprite.setTexture(_characterTexture);
    switch (p->_class)
    {
      case PlayerClass::kWizard: p->_sprite.setTextureRect(sf::IntRect(0, 2*8, 8, 8)); break;
      case PlayerClass::kRogue: p->_sprite.setTextureRect(sf::IntRect(3*8, 0, 8, 8)); break;
      case PlayerClass::kWarrior: p->_sprite.setTextureRect(sf::IntRect(15*8, 0, 8, 8)); break;
      case PlayerClass::kCleric: p->_sprite.setTextureRect(sf::IntRect(6*8, 0, 8, 8)); break;
    }
  }

  // Set the texture coords for the monster
  for (auto m : level->_monsters)
  {
    m->_sprite.setTexture(_characterTexture);
    switch (m->_type)
    {
      case MonsterType::kGoblin: m->_sprite.setTextureRect(sf::IntRect(0, 5*8, 8, 8)); break;
      case MonsterType::kSkeleton: m->_sprite.setTextureRect(sf::IntRect(0, 6*8, 8, 8)); break;
      case MonsterType::kSkeletonWarrior: m->_sprite.setTextureRect(sf::IntRect(2*8, 6*8, 8, 8)); break;
      case MonsterType::kSkeletonMage: m->_sprite.setTextureRect(sf::IntRect(3*8, 6*8, 8, 8)); break;
      case MonsterType::kFireElemental: m->_sprite.setTextureRect(sf::IntRect(0, 7*8, 8, 8)); break;
      case MonsterType::kWaterElemental: m->_sprite.setTextureRect(sf::IntRect(1*8, 7*8, 8, 8)); break;
      case MonsterType::kOgre: m->_sprite.setTextureRect(sf::IntRect(0, 8*8, 8, 8)); break;
      case MonsterType::kDemon: m->_sprite.setTextureRect(sf::IntRect(0, 9*8, 8, 8)); break;
    }
  }

  Resize();

  return true;
}
