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
  , _partyStatsWidth(200)
{
}

void Renderer::drawLevel() {

  Pos topLeft(_party->getActivePlayer()->_pos);

  auto size = _window->getSize();
  int partyWidth = 200;
  int zoom = 3;
  int rows = (size.y) / (zoom*8);
  int cols = (size.x - partyWidth) / (zoom*8);

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


  for (auto &monster : _level->_monsters)
    _window->draw(monster->_sprite);
}

void Renderer::drawParty() {

  Pos topLeft(_party->getActivePlayer()->_pos);

  auto size = _window->getSize();
  int partyWidth = 200;
  int zoom = 3;
  int tilesX = (size.x - partyWidth) / (zoom*8);
  int tilesY = (size.y) / (zoom*8);

  topLeft.row = max(0, topLeft.row - tilesY/2);
  topLeft.col = max(0, topLeft.col - tilesX/2);
  //_level->draw(_window, topLeft, tilesY, tilesX);

  auto *activePlayer = _party->_players[_party->_activePlayer];
  for (size_t i = 0; i < _party->_players.size(); ++i) {
    Player *cur = _party->_players[i];
    cur->_name = toString("Player %d", i);
    cur->_sprite.setPosition((float)(cur->_pos.col - topLeft.col)*3*8, (float)(cur->_pos.row - topLeft.row)*3*8);
    cur->_sprite.setColor(cur == activePlayer ? sf::Color(255, 255, 255) : sf::Color(127,127,127));
    _window->draw(cur->_sprite);
  }

}

void Renderer::drawPartyStats() {
  auto size = _window->getSize();
  float x = (float)(size.x - _partyStatsWidth);
  float y = 0;
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
    normal.setString(toString("(%s)", playerClassToString(player->_class).c_str()));
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

    drawHeading(player);

    drawNormal(toString("STR: %d", player->_strength));
    drawNormal(toString("INT: %d", player->_intelligence));
    drawNormal(toString("DEX: %d", player->_dexterity));
    drawNormal(toString("VIT: %d", player->_vitality));
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

  for (auto *p : _party->_players)
    p->_sprite.setTexture(_characterTexture);

  return true;
}
