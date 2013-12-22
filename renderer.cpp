#include "precompiled.hpp"
#include "renderer.hpp"
#include "player.hpp"
#include "utils.hpp"
#include "party.hpp"
#include "level.hpp"
#include "monster.hpp"
#include "game_state.hpp"

using namespace rogue;

namespace
{
  sf::Vertex MakeVertex(float x, float y)
  {
    return sf::Vertex(sf::Vector2f(x, y));
  }
}

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
  , _leftMargin(0)
  , _zoomLevel(3)
  , _rightMargin(210)
  , _topMargin(0)
  , _bottomMargin(0)
  , _offset(0,0)
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
  // Check that the active player is inside the currently visible area
  int rows, cols;
  ClampedVisibleArea(state._level, &rows, &cols);
  Rect rect(_offset, cols, rows);
  Player* player = state.GetActivePlayer();
  if (player && !rect.PointInside(player->_pos))
  {
    // Player outside the visible area, so center the view on him
    _offset.x = player->_pos.x - cols / 2;
    _offset.y = player->_pos.y - rows / 2;

    // Clamp the new position, so it's fully inside the visible area
    _offset.x = max(0, _offset.x);
    _offset.y = max(0, _offset.y);
    int w = state._level->Width();
    int h = state._level->Height();
    
    if (_offset.x + cols > w)
      _offset.x = w - cols;
    if (_offset.y + rows > h)
      _offset.y = h - rows;
  }

  DrawLevel(state);
  DrawMonsters(state);
  DrawParty(state);
  drawPartyStats(state);
}

//-----------------------------------------------------------------------------
void Renderer::ClampedVisibleArea(const Level* level, int* rows, int* cols) const
{
  VisibleArea(rows, cols);
  *rows = min(*rows, level->Height());
  *cols = min(*cols, level->Width());
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
void Renderer::Resize(const GameState& state)
{
  int rows, cols;
  ClampedVisibleArea(state._level, &rows, &cols);

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
  ClampedVisibleArea(state._level, &rows, &cols);

  Level* level = state._level;

  int idx = 0;
  for (int i = _offset.y; i < rows; ++i)
  {
    for (int j = _offset.x; j < cols; ++j)
    {
      Tile &tile = level->Get(i, j);
      sf::Sprite &sprite = _tileSprites[idx++];

      // If the tile is a wall, determine if it should be horizontal or vertical
      if (tile._type == TileType::kWall)
      {
        if (level->Inside(i+1, j) && level->Get(i+1, j)._type != TileType::kWall)
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

      if (tile._selected)
      {
        Pos org(ToLocal(Pos(i, j)));

        sf::Vertex verts[] = {
          MakeVertex(0 + (float)org.x, 0 + (float)org.y),
          MakeVertex(8 + (float)org.x, 0 + (float)org.y),
          MakeVertex(8 + (float)org.x, 8 + (float)org.y),
          MakeVertex(0 + (float)org.x, 8 + (float)org.y),
        };

        _window->draw(verts, 4, sf::LinesStrip);
      }

    }
  }
}

//-----------------------------------------------------------------------------
void Renderer::DrawParty(const GameState& state)
{
  Player* activePlayer = state.GetActivePlayer();
  assert(activePlayer);

  Party* party = state._party;

  int rows, cols;
  ClampedVisibleArea(state._level, &rows, &cols);
  Rect rect(_offset, cols, rows);

  for (size_t i = 0; i < party->_players.size(); ++i)
  {
    Player *player = party->_players[i];
    Pos pos(player->_pos);

    if (!rect.PointInside(pos))
      continue;

    Pos p(ToLocal(pos));

    player->_name = toString("Player %d", i);
    player->_sprite.setPosition((float)p.x*_zoomLevel*8, (float)p.y*_zoomLevel*8);
    player->_sprite.setColor(player == activePlayer ? sf::Color(255, 255, 255) : sf::Color(127,127,127));
    _window->draw(player->_sprite);

    //DrawHealthBar(player->_curHealth, player->_maxHeath, Pos(y, x));
  }
}

void Renderer::drawPartyStats(const GameState& state)
{
  Player* activePlayer = state.GetActivePlayer();
  assert(activePlayer);

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

//-----------------------------------------------------------------------------
void Renderer::DrawHealthBar(int health, int maxHealth, const Pos &pos)
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

//-----------------------------------------------------------------------------
Pos Renderer::ToLocal(const Pos& pos) const
{
  return pos - _offset;
}

//-----------------------------------------------------------------------------
Pos Renderer::ToGlobal(const Pos& pos) const
{
  return pos + _offset;
}

//-----------------------------------------------------------------------------
void Renderer::DrawMonsters(const GameState& state)
{
  Level* level = state._level;
  Party* party = state._party;

  int rows, cols;
  ClampedVisibleArea(state._level, &rows, &cols);
  Rect rect(_offset, cols, rows);

  auto size = _window->getSize();
  int zoom = 3;
  float zoomF = (float)zoom;

  for (auto monster : level->monsters())
  {
    if (!monster->_health)
      continue;

    Pos pos(ToLocal(monster->_pos));
    if (!rect.PointInside(pos))
      continue;

    monster->_sprite.setPosition(zoomF*pos.x*8, zoomF*pos.y*8);
    _window->draw(monster->_sprite);

    // draw the roam path
    vector<sf::Vertex> path;
    for (auto& p : monster->_roamPath)
    {
      Pos pp(ToLocal(p));
      path.push_back(sf::Vertex(sf::Vector2f(pp.x*8*zoomF, pp.y*8*zoomF)));
    }

    if (!path.empty())
      _window->draw(path.data(), path.size(), sf::LinesStrip);

    DrawHealthBar(monster->_health, monster->_maxHealth, pos);
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

  Resize(state);

  return true;
}
