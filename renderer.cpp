#include "renderer.hpp"
#include "player.hpp"
#include "utils.hpp"
#include "party.hpp"
#include "level.hpp"
#include "monster.hpp"
#include "game_state.hpp"
#include "game.hpp"
#include "window_event_manager.hpp"

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
  , _prevSelected(-1)
  , _offset(0,0)
  , _leftMargin(0)
  , _rightMargin(210)
  , _topMargin(0)
  , _bottomMargin(0)
  , _zoomLevel(3)
  , _debugDump(nullptr)
{
}

//-----------------------------------------------------------------------------
void Renderer::PlayerInView(const GameState& state)
{
  Player* player = state.GetActivePlayer();
  Pos pos(player->GetPos());
  Pos wsPos(PlayerToWorld(pos));

  // Make sure the current view contains the given player position
  auto level = state._level;
  auto& view = _rtMain.getView();
  auto center = view.getCenter();
  auto size = view.getSize();
  Rect rect((int)center.x - (int)size.x/2, (int)center.y - (int)size.y/2, (int)size.x, (int)size.y);

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
      center.x = (float)sx;
    }
    else if ((w - pos.x) * _zoomLevel * 8 < sx)
    {
      center.x = (float)(w * _zoomLevel * 8 - sx);
    }

    if (center.y < sy)
    {
      center.y = (float)sy;
    }
    else if ((h - pos.y) * _zoomLevel * 8 < sy)
    {
      center.y = (float)(h * _zoomLevel * 8 - sy);
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

  // display current state
  sf::Text text("", _font, 10);
  text.setString(state._description);
  text.setPosition(20, 200);
  text.setColor(Color::White);
  _window->draw(text);

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
  _sprCharacter.setPosition((float)(windowSize.x - _rightMargin), 0);
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

      if (!tile._items.empty())
      {
        _objectSprite.setTextureRect(_objectToTextureRect[tile._items.back()._type]);
        _objectSprite.setPosition(PlayerToWorldF(Pos(x,y)));
        _rtMain.draw(_objectSprite);
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
    Pos pos(player->GetPos());
    bool isActivePlayer = player == activePlayer;

    Color color(isActivePlayer ? sf::Color(255, 255, 255) : sf::Color(127,127,127));
    auto sprite = player->_sprite;
    sprite.SetPosition(PlayerToWorldF(pos));
    sprite.SetColor(color);
    sprite.SetHeading(player->_heading);
    _rtMain.draw(sprite);

    if (isActivePlayer)
    {
      Pos org(PlayerToWorld(pos));
      DrawQuad(org, 8, sf::Color::Green);
    }

    DrawHealthBar(player->CurHealth(), player->MaxHealth(), pos);
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
    heading.setString(player->Name());
    heading.setPosition(pos);
    sf::FloatRect r = heading.getLocalBounds();
    sf::Vector2f tmpPos = pos;
    pos.x += r.width + 10;
    _rtCharacterPane.draw(heading);
    normal.setString(toString("(%s, level %d)", playerClassToString(player->_class).c_str(), player->Level()));
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
    drawNormal(toString("HP: %d/%d", player->CurHealth(), player->MaxHealth()));
    if (player->MaxMana() > 0)
      drawNormal(toString("MANA: %d/%d", player->CurMana(), player->MaxMana()));

    drawNormal(toString("XP: %d/%d", player->_xp, player->_xpForNextLevel));
    drawNormal(toString("Gold: %d", player->_gold));

    pos.x = col1;
    pos.y = y;
    drawNormal(toString("STR: %d", player->_strength));
    drawNormal(toString("INT: %d", player->_intelligence));
    drawNormal(toString("DEX: %d", player->_dexterity));
    drawNormal(toString("VIT: %d", player->_vitality));
    drawNormal(toString("ARM: %d", player->Armor()));
    drawNormal(toString("+WEP: %d", player->WeaponBonus()));
    drawNormal(toString("+ARM: %d", player->ArmorBonus()));

    pos.y += 10;
  }

  // print debug info
  if (_debugDump)
  {
    pos.x = col0;
    vector<string> dump;
    _debugDump->DebugDump(dump);
    for (const auto& s : dump)
    {
      drawNormal(s.c_str());
    }
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
  return Vector2f((float)(pos.x * zoom), (float)(pos.y * zoom));
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
    if (!monster->CurHealth())
      continue;

    Pos pos(ToLocal(monster->GetPos()));
    if (!rect.contains(pos))
      continue;

    monster->_sprite.setPosition(PlayerToWorldF(monster->GetPos()));
    _rtMain.draw(monster->_sprite);

    DrawHealthBar(monster->CurHealth(), monster->MaxHealth(), pos);
  }
}

//-----------------------------------------------------------------------------
int Renderer::TileAtPos(const GameState& state, int x, int y) const
{
  auto level = state._level;
  size_t zoom = _zoomLevel * 8;

  //Vector2f wsPos = _rtMain.mapPixelToCoords(Pos(x,y));
  Vector2f wsPos = _rtMain.convertCoords(Pos(x,y));
  int tx = (int)(wsPos.x / zoom);
  int ty = (int)(wsPos.y / zoom);

  if (tx >= level->Width() || ty >= level->Height())
  {
    return -1;
  }

  return ty * level->Width() + tx;
}

//-----------------------------------------------------------------------------
void Renderer::OnMouseMove(const GameState& state, int x, int y, bool hover)
{
  auto level = state._level;

  if (_prevSelected != -1)
    level->_tiles[_prevSelected]._selected = false;

  int idx = TileAtPos(state, x, y);
  _prevSelected = idx;
  if (idx == -1)
    return;

  level->_tiles[idx]._selected = true;
}

//-----------------------------------------------------------------------------
bool Renderer::OnMouseButtonReleased(const Event& event)
{
  auto& state = GAME.GetGameState();
  int idx = TileAtPos(state, event.mouseButton.x, event.mouseButton.y);
  if (idx == -1)
    return false;

  auto& tile = state._level->_tiles[idx];

  if (tile._monster)
    _debugDump = tile._monster;

  if (tile._player)
    _debugDump = tile._player;

  return false;
}

//-----------------------------------------------------------------------------
bool Renderer::OnKeyPressed(const Event& event)
{
  if (event.key.code == Keyboard::Escape)
    _debugDump = nullptr;

  return false;
}

//-----------------------------------------------------------------------------
void Renderer::OnAttack(const GameEvent& event)
{
  // Add to combat log
  AddToCombatLog(toString("%s attacks %s for %d", event._agent->Name().c_str(), event._target->Name().c_str(), event._damage));

  // todo: start animation!
}

//-----------------------------------------------------------------------------
void Renderer::OnHeal(const GameEvent& event)
{

}

//-----------------------------------------------------------------------------
void Renderer::OnDeath(const GameEvent& event)
{

}

//-----------------------------------------------------------------------------
void Renderer::AddToCombatLog(const string& msg)
{
  _combatLog.push_back(msg);
}

//-----------------------------------------------------------------------------
bool Renderer::Init(const GameState& state)
{
  WINDOW_EVENT->RegisterHandler(Event::MouseButtonReleased,
      bind(&Renderer::OnMouseButtonReleased, this, _1));

  WINDOW_EVENT->RegisterHandler(Event::KeyReleased,
    bind(&Renderer::OnKeyPressed, this, _1));

  Level* level = state._level;
  Party* party = state._party;

  if (!_font.loadFromFile("gfx/wscsnrg.ttf"))
    return false;

  if (!_environmentTexture.loadFromFile("oryx_lofi/lofi_environment.png"))
    return false;

  if (!_characterTexture.loadFromFile("oryx_lofi/lofi_char.png"))
    return false;

  if (!_objectTexture.loadFromFile("oryx_lofi/lofi_obj.png"))
    return false;

  _objectToTextureRect[LootItem::Type::Gold]          = Rect(0,0,8,8);
  _objectToTextureRect[LootItem::Type::ManaPotion]    = Rect(13*8,0,8,8);
  _objectToTextureRect[LootItem::Type::HealthPotion]  = Rect(12*8,0,8,8);
  _objectToTextureRect[LootItem::Type::WeaponUpgrade] = Rect(5*8,3*8,8,8);
  _objectToTextureRect[LootItem::Type::ArmorUpgrade]  = Rect(4*8,4*8,8,8);

  _objectSprite.setTexture(_objectTexture);
  _objectSprite.setScale(3,3);

  // Set the texture coords for the players
  for (auto p : party->_players)
  {
    Rect textureRect;
    switch (p->_class)
    {
      case PlayerClass::kWizard: textureRect  = Rect(8*8, 29*8, 8, 8); break;
      case PlayerClass::kRanger: textureRect  = Rect(0,   29*8, 8, 8); break;
      case PlayerClass::kWarrior: textureRect = Rect(4*8, 29*8, 8, 8); break;
      case PlayerClass::kCleric: textureRect  = Rect(8*8, 30*8, 8, 8); break;
    }

    // sprites are stored E, S, W, N
    p->_sprite.Init(_characterTexture, 3,
      textureRect + Pos(8,0), textureRect, textureRect + Pos(24, 0), textureRect + Pos(16, 0));
  }

  // Set the texture coords for the monster
  for (auto m : level->_monsters)
  {
    m->_sprite.setTexture(_characterTexture);
    switch (m->GetMonsterType())
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
      if (tile._type == Tile::Type::kWall)
      {
        if (level->Inside(x,y+1) && level->Get(x,y+1)._type != Tile::Type::kWall)
        {
          sprite.setTextureRect(sf::IntRect((int)Tiles::wallH*8, 0, 8, 8));
        }
        else
        {
          sprite.setTextureRect(sf::IntRect((int)Tiles::wallV*8, 0, 8, 8));
        }
      }
      else if (tile._type == Tile::Type::kFloor)
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
