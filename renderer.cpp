#include "precompiled.hpp"
#include "renderer.hpp"
#include "player.hpp"
#include "utils.hpp"
#include "party.hpp"
#include "level.hpp"
#include "monster.hpp"

using namespace std;

enum class Tiles {
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


Renderer::Renderer(sf::RenderWindow *window) 
  : _window(window)
  , _partyStatsWidth(210)
  , _zoomLevel(3)
{
}

void Renderer::onMoveDone() {
  int a = 10;
}

void Renderer::drawWorld() {

  drawLevel();
  drawMonsters();
  drawParty();
  drawPartyStats();
}

void Renderer::drawLevel() {

  Pos topLeft(_party->getActivePlayer()->_pos);

  auto size = _window->getSize();
  int rows = (size.y) / (_zoomLevel*8);
  int cols = (size.x - _partyStatsWidth) / (_zoomLevel*8);

  topLeft.row = max(0, topLeft.row - rows/2);
  topLeft.col = max(0, topLeft.col - cols/2);

  // Recreate the sprites if the dimensions change
  if (_level->_tileSprites.size() != rows * cols) {
    _level->_tileSprites.resize(rows*cols);

    for (int i = 0; i < rows; ++i) {
      for (int j = 0; j < cols; ++j) {
        auto &sprite = _level->_tileSprites[i*cols+j];
        sprite.setPosition((float)j*24, (float)i*24);
        sprite.setScale(3.0f, 3.0f);
        sprite.setTexture(_environmentTexture);
      }
    }
  }

  int idx = 0;
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      int r = topLeft.row + i;
      int c = topLeft.col + j;
      if (!_level->inside(r, c))
        continue;
      Tile &tile = _level->get(r, c);
      sf::Sprite &sprite = _level->_tileSprites[idx++];

      if (tile._type == TileType::kWall) {
        if (_level->inside(r+1, c) && _level->get(r+1, c)._type != TileType::kWall) {
          sprite.setTextureRect(sf::IntRect((int)Tiles::wallH*8, 0, 8, 8));
        } else {
          sprite.setTextureRect(sf::IntRect((int)Tiles::wallV*8, 0, 8, 8));
        }
      } else if (tile._type == TileType::kFloor) {
        sprite.setTextureRect(sf::IntRect((int)Tiles::floorC*8, 0, 8, 8));

      } else {
        assert(false);
      }

      sprite.setColor(sf::Color(tile._visited, tile._visited, tile._visited));
      _window->draw(sprite);
    }
  }
}

void Renderer::drawParty() {

  Pos topLeft(_party->getActivePlayer()->_pos);

  auto size = _window->getSize();
  int cols = (size.x - _partyStatsWidth) / (_zoomLevel*8);
  int rows = (size.y) / (_zoomLevel*8);

  topLeft.row = max(0, topLeft.row - rows/2);
  topLeft.col = max(0, topLeft.col - cols/2);

  auto *activePlayer = _party->_players[_party->_activePlayer];
  for (size_t i = 0; i < _party->_players.size(); ++i) {
    Player *player = _party->_players[i];
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

void Renderer::drawPartyStats() {
  auto size = _window->getSize();
  float x = (float)(size.x - _partyStatsWidth);
  float col0 = x;
  float col1 = x + _partyStatsWidth/2;

  float y = 10;
  sf::Text heading("", _font, 20);
  sf::Text normal("", _font, 10);

  sf::Vector2f pos(x, y);

  auto &drawHeading = [&](Player *player) {
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

  auto &drawNormal = [&](const std::string &str) {
    normal.setString(str);
    normal.setPosition(pos);
    pos.y += 15;
    _window->draw(normal);
  };

  auto *activePlayer = _party->_players[_party->_activePlayer];
  for (auto *player : _party->_players) {

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

void Renderer::getVisibleArea(Pos *topLeft, int *rows, int *cols) {
/*
  auto *activePlayer = _party->getActivePlayer();
  Pos topLeft(activePlayer->_pos);

  auto size = _window->getSize();
  int _partyStatsWidth = 200;
  int zoom = 3;
  float zoomF = (float)zoom;
  int cols = (size.x - _partyStatsWidth) / (zoom*8);
  int rows = (size.y) / (zoom*8);

  topLeft.row = max(0, topLeft.row - rows/2);
  topLeft.col = max(0, topLeft.col - cols/2);
*/
}

void Renderer::drawHealthBar(int health, int maxHealth, const Pos &pos) {

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

void Renderer::drawMonsters() {

  auto *activePlayer = _party->getActivePlayer();
  Pos topLeft(activePlayer->_pos);

  auto size = _window->getSize();
  int zoom = 3;
  float zoomF = (float)zoom;
  int cols = (size.x - _partyStatsWidth) / (zoom*8);
  int rows = (size.y) / (zoom*8);

  topLeft.row = max(0, topLeft.row - rows/2);
  topLeft.col = max(0, topLeft.col - cols/2);

  for (auto *monster : _level->_monsters) {
    if (!monster->_health)
      continue;

    int x = monster->_pos.col - topLeft.col;
    int y = monster->_pos.row - topLeft.row;
    if (x < 0 || x >= cols || y < 0 || y >= rows)
      continue;
    monster->_sprite.setPosition(zoomF*x*8, zoomF*y*8);
    _window->draw(monster->_sprite);

    drawHealthBar(monster->_health, monster->_maxHealth, Pos(y, x));
  }
}

bool Renderer::init(Level *level, Party *party) {
  _level = level;
  _party = party;

  if (!_font.loadFromFile("gfx/wscsnrg.ttf"))
    return false;

  if (!_environmentTexture.loadFromFile("oryx_lofi/lofi_environment.png"))
    return false;

  if (!_characterTexture.loadFromFile("oryx_lofi/lofi_char.png"))
    return false;

  for (auto *p : _party->_players) {
    switch (p->_class) {
      case PlayerClass::kWizard: p->_sprite.setTextureRect(sf::IntRect(0, 2*8, 8, 8)); break;
      case PlayerClass::kRogue: p->_sprite.setTextureRect(sf::IntRect(3*8, 0, 8, 8)); break;
      case PlayerClass::kWarrior: p->_sprite.setTextureRect(sf::IntRect(15*8, 0, 8, 8)); break;
      case PlayerClass::kCleric: p->_sprite.setTextureRect(sf::IntRect(6*8, 0, 8, 8)); break;
    }
    p->_sprite.setTexture(_characterTexture);
  }

  for (auto *m : _level->_monsters) {
    switch (m->_type) {
      case MonsterType::kGoblin: m->_sprite.setTextureRect(sf::IntRect(0, 5*8, 8, 8)); break;
      case MonsterType::kSkeleton: m->_sprite.setTextureRect(sf::IntRect(0, 6*8, 8, 8)); break;
      case MonsterType::kSkeletonWarrior: m->_sprite.setTextureRect(sf::IntRect(2*8, 6*8, 8, 8)); break;
      case MonsterType::kSkeletonMage: m->_sprite.setTextureRect(sf::IntRect(3*8, 6*8, 8, 8)); break;
      case MonsterType::kFireElemental: m->_sprite.setTextureRect(sf::IntRect(0, 7*8, 8, 8)); break;
      case MonsterType::kWaterElemental: m->_sprite.setTextureRect(sf::IntRect(1*8, 7*8, 8, 8)); break;
      case MonsterType::kOgre: m->_sprite.setTextureRect(sf::IntRect(0, 8*8, 8, 8)); break;
      case MonsterType::kDemon: m->_sprite.setTextureRect(sf::IntRect(0, 9*8, 8, 8)); break;
    }
    m->_sprite.setTexture(_characterTexture);
  }

  return true;
}
