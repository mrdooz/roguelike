#include "stdafx.h"
#include "utils.hpp"

#include <vector>
#include <assert.h>

using namespace std;

enum Tiles {
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


struct Level {

  sf::Sprite &get(int row, int col) { return _sprites[row*_width+col]; }
  vector<sf::Sprite> _sprites;
  int _width;
  int _height;
};

class LevelFactory {

public:
  static bool create();
  static void close();
  static LevelFactory &instance();

  Level *makeLevel(int width, int height, const sf::Texture &texture);
private:
  static LevelFactory *_instance;
};

Level *LevelFactory::makeLevel(int width, int height, const sf::Texture &texture) {
  Level *level = new Level;

  level->_width = width;
  level->_height = height;

  level->_sprites.resize(width*height);

  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width; ++j) {
      auto &sprite = level->get(i, j);
      sprite.setPosition((float)j*24, (float)i*24);
      sprite.setScale(3.0f, 3.0f);
      sprite.setTextureRect(sf::IntRect(Tiles::floorA*8, 0, 8, 8));
      sprite.setTexture(texture);
    }
  }

  return level;
}

LevelFactory *LevelFactory::_instance;

bool LevelFactory::create() {
  assert(!_instance);
  _instance = new LevelFactory;
  return true;
}

void LevelFactory::close() {
  assert(_instance);
  delete exch_null(_instance);
}

LevelFactory &LevelFactory::instance() {
  assert(_instance);
  return *_instance;
}


class Game {

public:
  enum GameState {
    WaitingForPlayer,
    WaitingForWorld,
  };

  void init() {
  }

private:

  GameState _state;

};


class App {

public:

  bool close() {
    delete exch_null(_level);
    delete exch_null(_window);
    return true;
  }

  bool init() {
    findAppRoot();

    _window = new sf::RenderWindow(sf::VideoMode(800, 600), "SFML window");

    if (!_environmentTexture.loadFromFile("oryx_lofi\\lofi_environment.png"))
      return EXIT_FAILURE;

    LevelFactory::create();
    _level = LevelFactory::instance().makeLevel(20, 20, _environmentTexture);

    // create wall tiles
    for (int i = 0; i < Tiles::cNumTiles; ++i) {
      _wallSprites[i].setTexture(_environmentTexture);
      _wallSprites[i].setTextureRect(sf::IntRect(i*8,0,8,8));
    }

    return true;
  }

  int run()
  {

    // Create a graphical text to display
    sf::Font font;
    if (!font.loadFromFile("gfx\\wscsnrg.ttf"))
      return EXIT_FAILURE;
    sf::Text text("Hello SFML", font, 20);

    // Start the game loop
    while (_window->isOpen())
    {
      // Process events
      sf::Event event;
      while (_window->pollEvent(event)) {

        //sf::Keyboard::isKeyPressed()
        if (event.type == sf::Event::Closed)
          _window->close();
      }

      _window->clear();

      for (size_t i = 0; i < _level->_sprites.size(); ++i)
        _window->draw(_level->_sprites[i]);

      //_window->draw(text);

      _window->display();
    }

    return EXIT_SUCCESS;
  }

private:

  void findAppRoot()
  {
    char startingDir[MAX_PATH];
    _getcwd(startingDir, MAX_PATH);

    // keep going up directory levels until we find "app.json", or we hit the bottom..
    char prevDir[MAX_PATH], curDir[MAX_PATH];
    ZeroMemory(prevDir, sizeof(prevDir));

    while (true) {
      _getcwd(curDir, MAX_PATH);
      // check if we haven't moved
      if (!strcmp(curDir, prevDir))
        break;

      memcpy(prevDir, curDir, MAX_PATH);

      if (fileExists("settings.ini")) {
        _appRoot = curDir;
        return;
      }

      if (_chdir("..") == -1)
        break;
    }
    _appRoot = startingDir;
  }

  Level *_level;

  sf::Texture _environmentTexture;
  sf::Sprite _wallSprites[Tiles::cNumTiles];
  sf::RenderWindow *_window;

  string _appRoot;

};


int main() {
  App app;
  if (!app.init())
    return 1;

  int res = app.run();

  app.close();

  if (res) {
    while (!isBitSet(GetAsyncKeyState(VK_ESCAPE), 15))
      ;
  }

  return res;
}
