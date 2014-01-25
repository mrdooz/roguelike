#include "renderer.hpp"
#include "player.hpp"
#include "utils.hpp"
#include "party.hpp"
#include "level.hpp"
#include "monster.hpp"
#include "game_state.hpp"
#include "game.hpp"
#include "window_event_manager.hpp"
#include "animation_manager.hpp"

using namespace rogue;
using namespace sf;

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
MainWindow::MainWindow(const string& title, const Vector2f& pos, const Vector2f& size, Renderer* renderer)
  : VirtualWindow(title, pos, size)
  , _renderer(renderer)
  , _zoomLevel(3)
  , _offset(0,0)
{
}

//-----------------------------------------------------------------------------
bool MainWindow::Init()
{
  VirtualWindowManager* mgr = &_renderer->_windowManager;
  mgr->RegisterHandler(Event::Resized, this, bind(&MainWindow::OnResize, this, _1));
  mgr->RegisterHandler(Event::MouseMoved, this, bind(&MainWindow::OnMouseMove, this, _1));

  mgr->RegisterHandler(Event::MouseButtonReleased, this, bind(&MainWindow::OnMouseButtonReleased, this, _1));
  mgr->RegisterHandler(Event::KeyReleased, this, bind(&MainWindow::OnKeyPressed, this, _1));

  return true;
}

//-----------------------------------------------------------------------------
void MainWindow::PlayerInView()
{
  const GameState& state = GAME->GetGameState();
  Player* player = state.GetActivePlayer();
  Pos pos(player->GetPos());
  Pos wsPos(PlayerToWorld(pos));

  // Make sure the current view contains the given player position
  auto level = state._level;
  auto& view = _texture.getView();
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
    auto windowSize = _texture.getSize();
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

    _texture.setView(View(center, VectorCast<float>(windowSize)));
  }
}

//-----------------------------------------------------------------------------
Pos MainWindow::ToLocal(const Pos& pos) const
{
  return pos - _offset;
}

//-----------------------------------------------------------------------------
Pos MainWindow::ToGlobal(const Pos& pos) const
{
  return pos + _offset;
}

//-----------------------------------------------------------------------------
Pos MainWindow::PlayerToWorld(const Pos& pos) const
{
  size_t zoom = _zoomLevel * 8;
  return Pos(pos.x * zoom, pos.y * zoom);
}

//-----------------------------------------------------------------------------
Vector2f MainWindow::PlayerToWorldF(const Pos& pos) const
{
  size_t zoom = _zoomLevel * 8;
  return Vector2f((float)(pos.x * zoom), (float)(pos.y * zoom));
}

//-----------------------------------------------------------------------------
bool MainWindow::OnMouseMove(const Event& event)
{
  const GameState& state = GAME->GetGameState();
  int x = event.mouseMove.x;
  int y = event.mouseMove.y;
  auto level = state._level;

  if (_renderer->_prevSelected != -1)
    level->Get(_renderer->_prevSelected)->_selected = false;

  int idx = TileAtPos(state, x, y);
  _renderer->_prevSelected = idx;
  if (idx == -1)
    return false;

  level->Get(idx)->_selected = true;

  return false;
}

//-----------------------------------------------------------------------------
bool MainWindow::OnMouseButtonReleased(const Event& event)
{
  int x = event.mouseButton.x;
  int y = event.mouseButton.y;

  const GameState& state = GAME->GetGameState();
  Level* level = state._level;
  size_t zoom = _zoomLevel * 8;

  Vector2f wsPos = _texture.mapPixelToCoords(Pos(x,y));
  int tx = (int)(wsPos.x / zoom);
  int ty = (int)(wsPos.y / zoom);

  if (tx >= level->Width() || ty >= level->Height())
  {
    return false;
  }

  Tile& tile = level->Get(tx, ty);

  SelectionEvent selEvent(&tile);

  if (tile._monster)
  {
    selEvent._entity = tile._monster;
    _renderer->_debugDump = tile._monster;
  }

  if (tile._player)
  {
    selEvent._entity = tile._player;
    _renderer->_debugDump = tile._player;
  }

  GAME->PostSelectionEvent(selEvent);
  return false;
}

//-----------------------------------------------------------------------------
bool MainWindow::OnResize(const Event& event)
{
  return false;
}

//-----------------------------------------------------------------------------
bool MainWindow::OnKeyPressed(const Event& event)
{
  if (event.key.code == Keyboard::Escape)
    _renderer->_debugDump = nullptr;

  return false;
}

//-----------------------------------------------------------------------------
void MainWindow::Draw()
{
  _texture.clear();

  DrawLevel();
  DrawMonsters();
  DrawParty();
  DrawAnimations();

  if (_renderer->_debugDump)
    _renderer->_debugDump->DebugDraw(_texture);

  // Blit render target to render window
  _texture.display();
}

//-----------------------------------------------------------------------------
void MainWindow::DrawQuad(const Pos& topLeft, u32 size, Color color)
{
  int s = size * _zoomLevel;
  Vertex verts[] = {
    MakeVertex(0 + topLeft.x, 0 + topLeft.y, color),
    MakeVertex(s + topLeft.x, 0 + topLeft.y, color),
    MakeVertex(s + topLeft.x, s + topLeft.y, color),
    MakeVertex(0 + topLeft.x, s + topLeft.y, color),
    MakeVertex(0 + topLeft.x, 0 + topLeft.y, color),
  };

  _texture.draw(verts, 5, LinesStrip);
}

//-----------------------------------------------------------------------------
void MainWindow::VisibleArea(const Level* level, int* rows, int* cols) const
{
  // Determine number of tiles in the visible area (rows x cols)
  auto size = _texture.getSize();
  size_t zoom = _zoomLevel * 8;

  *cols = min(1 + (int)(size.x / zoom), level->Width());
  *rows = min(1 + (int)(size.y / zoom), level->Height());
}

//-----------------------------------------------------------------------------
void MainWindow::DrawLevel()
{
  const GameState& state = GAME->GetGameState();
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
      auto& sprite = _renderer->_tileSprites[y*level->Width()+x];

      //      sprite.setColor(Color(tile._visited, tile._visited, tile._visited));
      _texture.draw(sprite);

      if (tile._selected)
      {
        selectedTile = &tile;
        selectedPos = PlayerToWorld(Pos(x,y));
      }

      if (!tile._items.empty())
      {
        _renderer->_objectSprite.setTextureRect(_renderer->_objectToTextureRect[tile._items.back()._type]);
        _renderer->_objectSprite.setPosition(PlayerToWorldF(Pos(x,y)));
        _texture.draw(_renderer->_objectSprite);
      }
    }
  }

  if (selectedTile)
    DrawQuad(selectedPos, 8, Color::White);
}

//-----------------------------------------------------------------------------
void MainWindow::DrawParty()
{
  const GameState& state = GAME->GetGameState();
  Player* activePlayer = state.GetActivePlayer();
  assert(activePlayer);

  Party* party = state._party;

  int rows, cols;
  VisibleArea(state._level, &rows, &cols);
  Rect rect(_offset, Pos(cols, rows));

  for (size_t i = 0; i < party->_players.size(); ++i)
  {
    PlayerPtr player = party->_players[i];
    Pos pos(player->GetPos());
    bool isActivePlayer = player == activePlayer;

    Color color(isActivePlayer ? Color(255, 255, 255) : Color(127,127,127));
    auto sprite = player->_sprite;
    sprite.SetPosition(PlayerToWorldF(pos));
    sprite.SetColor(color);
    sprite.SetHeading(player->_heading);
    _texture.draw(sprite);

    if (isActivePlayer)
    {
      Pos org(PlayerToWorld(pos));
      DrawQuad(org, 8, Color::Green);
    }

    DrawHealthBar(player->CurHealth(), player->MaxHealth(), pos);
  }
}

//-----------------------------------------------------------------------------
void MainWindow::DrawHealthBar(int health, int maxHealth, const Pos &pos)
{
  float zoomF = (float)_zoomLevel;

  RectangleShape rectangle;
  // deficit
  rectangle.setSize(Vector2f(zoomF*6, zoomF));
  rectangle.setPosition((pos.x*8+1)*zoomF, (pos.y*8+7)*zoomF);
  rectangle.setFillColor(Color(200, 10, 10));
  _texture.draw(rectangle);

  // cur health
  rectangle.setSize(Vector2f((float)health / maxHealth*zoomF*6, zoomF));
  rectangle.setPosition((pos.x*8+1)*zoomF, (pos.y*8+7)*zoomF);
  rectangle.setFillColor(Color(10, 200, 10));
  _texture.draw(rectangle);
}

//-----------------------------------------------------------------------------
void MainWindow::DrawMonsters()
{
  const GameState& state = GAME->GetGameState();
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
    _texture.draw(monster->_sprite);

    DrawHealthBar(monster->CurHealth(), monster->MaxHealth(), pos);
  }
}

//-----------------------------------------------------------------------------
int MainWindow::TileAtPos(const GameState& state, int x, int y) const
{
  auto level = state._level;
  size_t zoom = _zoomLevel * 8;

  Vector2f wsPos = _texture.mapPixelToCoords(Pos(x,y));
  //  Vector2f wsPos = _rtMain->convertCoords(Pos(x,y));
  int tx = (int)(wsPos.x / zoom);
  int ty = (int)(wsPos.y / zoom);

  if (tx >= level->Width() || ty >= level->Height())
  {
    return -1;
  }

  return ty * level->Width() + tx;
}

//-----------------------------------------------------------------------------
void MainWindow::DrawAnimations()
{
  auto now = microsec_clock::local_time();

  for (auto it = _renderer->_activeAnimations.begin(); it != _renderer->_activeAnimations.end(); )
  {
    auto& instance = *it;
    const Animation* animation = instance._animation;

    // Check if the animation has ended; otherwise draw it
    time_duration remaining = instance._endTime - now;
    if (remaining.total_milliseconds() <= 0)
    {
      it = _renderer->_activeAnimations.erase(it);
    }
    else
    {
      float ratio = 1 - (float)remaining.total_milliseconds() / instance._duration.total_milliseconds();
      size_t frameIdx = animation->_frameIndex[(size_t)(animation->_weightSum * ratio)];
      auto& sprite = instance._sprite;
      sprite.setTextureRect(animation->_frames[frameIdx]._textureRect);
      sprite.setPosition(VectorCast<float>(lerp(instance._startPos, instance._endPos, ratio)));
      _texture.draw(sprite);
      ++it;
    }
  }
}

//-----------------------------------------------------------------------------
CombatLogWindow::CombatLogWindow(const string& title, const Vector2f& pos, const Vector2f& size, Renderer* renderer)
  : VirtualWindow(title, pos, size)
  , _renderer(renderer)
{
}

//-----------------------------------------------------------------------------
void CombatLogWindow::Draw()
{
  _texture.clear();

  RectangleShape background(Vector2f(_size.x, _size.y));
  background.setFillColor(Color(40,40,40));
  _texture.draw(background);

  int spacing = 15;
  Text normal("", _renderer->_font, 10);
  Vector2f pos(0, 0);

  int rows = min((int)_renderer->_combatLog.size(), (int)(_size.y - 20) / spacing);
  for (int i = 0; i < rows; ++i)
  {
    const string& cur = _renderer->_combatLog[_renderer->_combatLog.size() - rows + i];
    normal.setString(cur);
    normal.setPosition(pos);
    _texture.draw(normal);
    pos.y += spacing;
  }
  _texture.display();
}

//-----------------------------------------------------------------------------
PartyWindow::PartyWindow(const string& title, const Vector2f& pos, const Vector2f& size, Renderer* renderer)
  : VirtualWindow(title, pos, size)
  , _renderer(renderer)
{
}

//-----------------------------------------------------------------------------
void PartyWindow::Draw()
{
  _texture.clear();

  const GameState& state = GAME->GetGameState();

  Font& font = _renderer->_font;

  Player* activePlayer = state.GetActivePlayer();
  assert(activePlayer);

  Party* party = state._party;

  float x = 0;
  float col0 = x;
  float col1 = x + _size.x/2;

  float y = 10;
  Text heading("", font, 20);
  Text normal("", font, 10);

  Vector2f pos(x, y);

  auto drawHeading = [&](Player *player)
  {
    heading.setString(player->Name());
    heading.setPosition(pos);
    FloatRect r = heading.getLocalBounds();
    Vector2f tmpPos = pos;
    pos.x += r.width + 10;
    _texture.draw(heading);
    normal.setString(toString("(%s, level %d)", playerClassToString(player->_class).c_str(), player->Level()));
    pos.y += (r.height - normal.getLocalBounds().height) / 2;
    normal.setPosition(pos);
    _texture.draw(normal);
    pos = tmpPos;
    pos.y += 25;
  };

  auto drawNormal = [&](const std::string &str)
  {
    normal.setString(str);
    normal.setPosition(pos);
    pos.y += 15;
    _texture.draw(normal);
  };

  for (PlayerPtr& player : party->_players)
  {
    heading.setColor(player == activePlayer ? Color(255, 255, 255) : Color(127,127,127));
    normal.setColor(player == activePlayer ? Color(255, 255, 255) : Color(127,127,127));

    pos.x = col0;
    drawHeading(player.get());
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
  if (_renderer->_debugDump)
  {
    pos.x = col0;
    vector<string> dump;
    _renderer->_debugDump->DebugDump(dump);
    for (const auto& s : dump)
    {
      drawNormal(s.c_str());
    }
  }
  _texture.display();
}

//-----------------------------------------------------------------------------
Renderer::Renderer(
    RenderWindow *window,
    WindowEventManager* eventManager,
    AnimationManager* animationManager)
  : _window(window)
  , _prevSelected(-1)
  , _windowManager(window, eventManager)
  , _debugDump(nullptr)
  , _mainWindow(new MainWindow("MAIN", Vector2f(0,10), Vector2f(600, 600), this))
  , _combatLogWindow(new CombatLogWindow("COMBAT LOG", Vector2f(0,600), Vector2f(600, 200), this))
  , _partyWindow(new PartyWindow("PARTY", Vector2f(600,10), Vector2f(200, 600), this))
  , _animationManager(animationManager)
{
  _windowManager.AddWindow(_mainWindow);
  _windowManager.AddWindow(_combatLogWindow);
  _windowManager.AddWindow(_partyWindow);
}

//-----------------------------------------------------------------------------
Renderer::~Renderer()
{
}

//-----------------------------------------------------------------------------
void Renderer::DrawWorld(const GameState& state)
{
  // display current state
  Text text("", _font, 10);
  text.setString(state._description);
  text.setPosition(20, 200);
  text.setColor(Color::White);
  _window->draw(text);

  _windowManager.Update();
}

//-----------------------------------------------------------------------------
int Renderer::TileAtPos(const GameState& state, int x, int y) const
{
  return _mainWindow->TileAtPos(state, x, y);
}

//-----------------------------------------------------------------------------
void Renderer::OnAttack(const GameEvent& event)
{
  // Add to combat log
  AddToCombatLog(toString("%s attacks %s for %d",
    event._agent->Name().c_str(), event._target->Name().c_str(), event._damage));
}

//-----------------------------------------------------------------------------
void Renderer::OnHeal(const GameEvent& event)
{
  AddToCombatLog(toString("%s heals %s for %d",
    event._agent->Name().c_str(), event._target->Name().c_str(), event._healing));
}

//-----------------------------------------------------------------------------
void Renderer::OnDeath(const GameEvent& event)
{
  if (event._agent)
    AddToCombatLog(toString("%s is killed by %s!", event._target->Name().c_str(), event._agent->Name().c_str()));
  else
    AddToCombatLog(toString("%s dies!", event._target->Name().c_str()));
}

//-----------------------------------------------------------------------------
void Renderer::OnItemGained(const GameEvent& event)
{
  AddToCombatLog(toString("%s gains %s!", event._target->Name().c_str(), event._item->ToString().c_str()));
}

//-----------------------------------------------------------------------------
void Renderer::OnLevelGained(const GameEvent& event)
{
  AddToCombatLog(toString("%s gains a level!", event._target->Name().c_str()));
}

//-----------------------------------------------------------------------------
void Renderer::AddToCombatLog(const string& msg)
{
  _combatLog.push_back(msg);
}

//-----------------------------------------------------------------------------
bool Renderer::Init(const GameState& state)
{
  GAME_EVENT->RegisterHandler(GameEvent::Type::Attack, bind(&Renderer::OnAttack, this, _1));
  GAME_EVENT->RegisterHandler(GameEvent::Type::Heal, bind(&Renderer::OnHeal, this, _1));
  GAME_EVENT->RegisterHandler(GameEvent::Type::Death, bind(&Renderer::OnDeath, this, _1));
  GAME_EVENT->RegisterHandler(GameEvent::Type::ItemGained, bind(&Renderer::OnItemGained, this, _1));
  GAME_EVENT->RegisterHandler(GameEvent::Type::LevelGained, bind(&Renderer::OnLevelGained, this, _1));

  Level* level = state._level;
  Party* party = state._party;

  if (!_font.loadFromFile("gfx/wscsnrg.ttf"))
    return false;

  if (!(_environmentTexture = TextureCache::Instance()->LoadTextureByHandle("oryx_lofi/lofi_environment.png")))
    return false;

  if (!(_characterTexture = TextureCache::Instance()->LoadTextureByHandle("oryx_lofi/lofi_char.png")))
    return false;

  if (!(_objectTexture = TextureCache::Instance()->LoadTextureByHandle("oryx_lofi/lofi_obj.png")))
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
    switch (m->GetType())
    {
      case Monster::Type::Goblin: m->_sprite.setTextureRect(IntRect(0, 5*8, 8, 8)); break;
      case Monster::Type::Skeleton: m->_sprite.setTextureRect(IntRect(0, 6*8, 8, 8)); break;
      case Monster::Type::SkeletonWarrior: m->_sprite.setTextureRect(IntRect(2*8, 6*8, 8, 8)); break;
      case Monster::Type::SkeletonMage: m->_sprite.setTextureRect(IntRect(3*8, 6*8, 8, 8)); break;
      case Monster::Type::FireElemental: m->_sprite.setTextureRect(IntRect(0, 7*8, 8, 8)); break;
      case Monster::Type::WaterElemental: m->_sprite.setTextureRect(IntRect(1*8, 7*8, 8, 8)); break;
      case Monster::Type::Ogre: m->_sprite.setTextureRect(IntRect(0, 8*8, 8, 8)); break;
      case Monster::Type::Demon: m->_sprite.setTextureRect(IntRect(0, 9*8, 8, 8)); break;
    }
  }

  // For now, create a sprite per tile in the level
  int height = level->Height();
  int width = level->Width();
  _tileSprites.resize(width*height);
  //size_t zoom = _zoomLevel * 8;
  size_t zoom = 3 * 8;
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
      if (tile._type == Tile::Type::Wall)
      {
        if (level->Inside(x,y+1) && level->Get(x,y+1)._type != Tile::Type::Wall)
        {
          sprite.setTextureRect(IntRect((int)Tiles::wallH*8, 0, 8, 8));
        }
        else
        {
          sprite.setTextureRect(IntRect((int)Tiles::wallV*8, 0, 8, 8));
        }
      }
      else if (tile._type == Tile::Type::Floor)
      {
        sprite.setTextureRect(IntRect((int)Tiles::floorC*8, 0, 8, 8));
      }
      else
      {
        assert(false);
      }
    }
  }

  if (!_windowManager.Init())
    return false;

  return true;
}

//-----------------------------------------------------------------------------
void Renderer::AddAnimation(
    Animation::Id id,
    const Pos& startPos,
    const Pos& endPos,
    const time_duration& duration)
{
  Animation* animation = _animationManager->FindAnimation(id);
  if (!animation)
  {
    return;
  }

  AnimationInstance instance;
  instance._animation = animation;
  instance._startTime = microsec_clock::local_time();
  instance._endTime = instance._startTime + duration;
  instance._startPos = startPos;
  instance._endPos = endPos;
  instance._duration = duration;
  instance._sprite.setTexture(animation->_texture);
  _activeAnimations.push_back(instance);
}
