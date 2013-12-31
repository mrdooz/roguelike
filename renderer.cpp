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
  sf::Vertex MakeVertex(int x, int y, sf::Color color = sf::Color::White)
  {
    return sf::Vertex(sf::Vector2f((float)x, (float)y), color);
  }

  template <typename To, typename From>
  sf::Vector2<To> VectorCast(const sf::Vector2<From>& src)
  {
    return sf::Vector2<To>((To)src.x, (To)src.y);
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
  , _prevSelected(-1)
  , _offset(0,0)
  , _leftMargin(0)
  , _rightMargin(210)
  , _topMargin(0)
  , _bottomMargin(0)
  , _zoomLevel(3)
{
}

//-----------------------------------------------------------------------------
void Renderer::PlayerInView(const GameState& state)
{
  Player* player = state.GetActivePlayer();
  Pos pos(player->_pos);
  Pos wsPos(PlayerToWorld(pos));

  // Make sure the current view contains the given player position
  auto level = state._level;
  auto& view = _rtMain.getView();
  auto center = view.getCenter();
  auto size = view.getSize();
  Rect rect(center.x - size.x/2, center.y - size.y/2, size.x, size.y);

  // Check that all four corners of the player are inside the window
  int s = _zoomLevel * 8;
  bool inside = rect.contains(wsPos + s * Pos(0, 0))
    && rect.contains(wsPos + s * Pos(1, 0))
    && rect.contains(wsPos + s * Pos(0, 1))
    && rect.contains(wsPos + s * Pos(1, 1));

  if (!inside)
  {
    auto windowSize = _rtMain.getSize();
    Vector2f center(VectorCast<float>(wsPos));

    // If the player position is less than half the window away from the corners,
    // bump it to avoid leaving empty space
    int w = level->Width();
    int h = level->Height();
    int sx = windowSize.x / 2;
    int sy = windowSize.y / 2;
    if (center.x < windowSize.x/2)
    {
      center.x = sx;
    }
    else if ((w - pos.x) * _zoomLevel * 8 < sx)
    {
      center.x = w * _zoomLevel * 8 - sx;
    }

    if (center.y < sy)
    {
      center.y = sy;
    }
    else if ((h - pos.y) * _zoomLevel * 8 < sy)
    {
      center.y = h * _zoomLevel * 8 - sy;
    }

    _rtMain.setView(View(center, VectorCast<float>(windowSize)));
  }
}

//-----------------------------------------------------------------------------
void Renderer::DrawWorld(const GameState& state)
{
  PlayerInView(state);

  // Render to render target
  _rtMain.clear();

  DrawLevel(state);
  DrawMonsters(state);
  DrawParty(state);

  // Blit render target to render window
  _rtMain.display();
  _window->draw(_sprMain);

  // Render to char rt
  _rtCharacterPane.clear();
  DrawPartyStats(state);
  _rtCharacterPane.display();
  _window->draw(_sprCharacter);

}

//-----------------------------------------------------------------------------
void Renderer::VisibleArea(const Level* level, int* rows, int* cols) const
{
  // Determine number of tiles in the visible area (rows x cols)
  auto size = _rtMain.getSize();
  size_t zoom = _zoomLevel * 8;

  *cols = min(1 + (int)(size.x / zoom), level->Width());
  *rows = min(1 + (int)(size.y / zoom), level->Height());
}

//-----------------------------------------------------------------------------
void Renderer::Resize(const GameState& state)
{
  auto windowSize = _window->getSize();

  _rtMain.create(windowSize.x - _rightMargin, windowSize.y - _topMargin);
  _sprMain.setTexture(_rtMain.getTexture());

  _rtCharacterPane.create(_rightMargin, windowSize.y - _topMargin);
  _sprCharacter.setTexture(_rtCharacterPane.getTexture());
  _sprCharacter.setPosition(windowSize.x - _rightMargin, 0);
}

//-----------------------------------------------------------------------------
void Renderer::DrawQuad(const Pos& topLeft, size_t size, sf::Color color)
{
  int s = size * _zoomLevel;
  sf::Vertex verts[] = {
    MakeVertex(0 + topLeft.x, 0 + topLeft.y, color),
    MakeVertex(s + topLeft.x, 0 + topLeft.y, color),
    MakeVertex(s + topLeft.x, s + topLeft.y, color),
    MakeVertex(0 + topLeft.x, s + topLeft.y, color),
    MakeVertex(0 + topLeft.x, 0 + topLeft.y, color),
  };

  _rtMain.draw(verts, 5, sf::LinesStrip);
}

//-----------------------------------------------------------------------------
void Renderer::DrawLevel(const GameState& state)
{
  int rows, cols;
  VisibleArea(state._level, &rows, &cols);

  Level* level = state._level;
  Tile* selectedTile = nullptr;
  Pos selectedPos;

  for (int y = 0; y < level->Height(); ++y)
  {
    for (int x = 0; x < level->Width(); ++x)
    {
      Tile &tile = level->Get(x,y);
      sf::Sprite& sprite = _tileSprites[y*level->Width()+x];

//      sprite.setColor(sf::Color(tile._visited, tile._visited, tile._visited));
      _rtMain.draw(sprite);

      if (tile._selected)
      {
        selectedTile = &tile;
        selectedPos = PlayerToWorld(Pos(x,y));
      }
    }
  }

  if (selectedTile)
    DrawQuad(selectedPos, 8, sf::Color::White);
}

//-----------------------------------------------------------------------------
void Renderer::DrawParty(const GameState& state)
{
  Player* activePlayer = state.GetActivePlayer();
  assert(activePlayer);

  Party* party = state._party;

  int rows, cols;
  VisibleArea(state._level, &rows, &cols);
  Rect rect(_offset, Pos(cols, rows));

  for (size_t i = 0; i < party->_players.size(); ++i)
  {
    Player *player = party->_players[i];
    Pos pos(player->_pos);
    bool isActivePlayer = player == activePlayer;

    player->_name = toString("Player %d", i);
    player->_sprite.setPosition(PlayerToWorldF(pos));
    player->_sprite.setColor(isActivePlayer ? sf::Color(255, 255, 255) : sf::Color(127,127,127));
    _rtMain.draw(player->_sprite);

    if (isActivePlayer)
    {
      Pos org(PlayerToWorld(pos));
      DrawQuad(org, 8, sf::Color::Green);
    }

    //DrawHealthBar(player->_curHealth, player->_maxHeath, Pos(y, x));
  }
}

//-----------------------------------------------------------------------------
void Renderer::DrawPartyStats(const GameState& state)
{
  Player* activePlayer = state.GetActivePlayer();
  assert(activePlayer);

  Party* party = state._party;

  float x = 0;
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
    _rtCharacterPane.draw(heading);
    normal.setString(toString("(%s, level %d)", playerClassToString(player->_class).c_str(), player->_level));
    pos.y += (r.height - normal.getLocalBounds().height) / 2;
    normal.setPosition(pos);
    _rtCharacterPane.draw(normal);
    pos = tmpPos;
    pos.y += 25;
  };

  auto drawNormal = [&](const std::string &str)
  {
    normal.setString(str);
    normal.setPosition(pos);
    pos.y += 15;
    _rtCharacterPane.draw(normal);
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
  rectangle.setPosition((pos.x*8+1)*zoomF, (pos.y*8+7)*zoomF);
  rectangle.setFillColor(sf::Color(200, 10, 10));
  _rtMain.draw(rectangle);
  // cur health
  rectangle.setSize(sf::Vector2f((float)health / maxHealth*zoomF*6, zoomF));
  rectangle.setPosition((pos.x*8+1)*zoomF, (pos.y*8+7)*zoomF);
  rectangle.setFillColor(sf::Color(10, 200, 10));
  _rtMain.draw(rectangle);

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
Pos Renderer::PlayerToWorld(const Pos& pos) const
{
  size_t zoom = _zoomLevel * 8;
  return Pos(pos.x * zoom, pos.y * zoom);
}

//-----------------------------------------------------------------------------
Vector2f Renderer::PlayerToWorldF(const Pos& pos) const
{
  size_t zoom = _zoomLevel * 8;
  return Vector2f(pos.x * zoom, pos.y * zoom);
}

//-----------------------------------------------------------------------------
void Renderer::DrawMonsters(const GameState& state)
{
  Level* level = state._level;

  int rows, cols;
  VisibleArea(state._level, &rows, &cols);
  Rect rect(_offset, Pos(cols, rows));

  for (auto monster : level->monsters())
  {
    if (!monster->_health)
      continue;

    Pos pos(ToLocal(monster->_pos));
    if (!rect.contains(pos))
      continue;

    monster->_sprite.setPosition(PlayerToWorldF(monster->_pos));
    _rtMain.draw(monster->_sprite);

    // draw the roam path
    vector<sf::Vertex> path;
    for (auto& p : monster->_roamPath)
    {
      path.push_back(PlayerToWorldF(p));
    }

    if (!path.empty())
      _rtMain.draw(path.data(), path.size(), sf::LinesStrip);

    DrawHealthBar(monster->_health, monster->_maxHealth, pos);
  }
}

//-----------------------------------------------------------------------------
void Renderer::OnMouseMove(const GameState& state, int x, int y, bool hover)
{
  auto level = state._level;

  if (_prevSelected != -1)
    level->_tiles[_prevSelected]._selected = false;

  size_t zoom = _zoomLevel * 8;

  Vector2f wsPos = _rtMain.mapPixelToCoords(Pos(x,y));
  int tx = wsPos.x / zoom;
  int ty = wsPos.y / zoom;

  if (tx >= level->Width() || ty >= level->Height())
  {
    _prevSelected = -1;
    return;
  }

  int idx = ty * level->Width() + tx;
  _prevSelected = idx;
  level->_tiles[idx]._selected = true;
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
      case PlayerClass::kRanger: p->_sprite.setTextureRect(sf::IntRect(3*8, 0, 8, 8)); break;
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

  // For now, create a sprite per tile in the level
  int height = level->Height();
  int width = level->Width();
  _tileSprites.resize(width*height);
  size_t zoom = _zoomLevel * 8;
  for (int y = 0; y < height; ++y)
  {
    for (int x = 0; x < width; ++x)
    {
      auto& sprite = _tileSprites[y*width+x];
      sprite.setPosition((float)x*zoom, (float)y*zoom);
      sprite.setScale(3.0f, 3.0f);
      sprite.setTexture(_environmentTexture);

      auto& tile = level->Get(x, y);

      // If the tile is a wall, determine if it should be horizontal or vertical
      if (tile._type == TileType::kWall)
      {
        if (level->Inside(x,y+1) && level->Get(x,y+1)._type != TileType::kWall)
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
    }
  }

  Resize(state);

  return true;
}
