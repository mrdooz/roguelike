#include "stdafx.h"
#include "utils.hpp"

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

  sf::Texture _wallTexture;
  sf::Sprite _wallSprites[Tiles::cNumTiles];
  sf::RenderWindow *_window;

public:

  bool close() {
    delete _window;
    return true;
  }

  bool init() {
    findAppRoot();

    _window = new sf::RenderWindow(sf::VideoMode(800, 600), "SFML window");

    if (!_wallTexture.loadFromFile("oryx_lofi\\lofi_environment.png"))
      return EXIT_FAILURE;

    // create wall tiles
    for (int i = 0; i < Tiles::cNumTiles; ++i) {
      _wallSprites[i].setTexture(_wallTexture);
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

      for (int i = 0; i < 20; ++i) {
        _wallSprites[Tiles::wallH].setPosition(100+i*8, 100);
        _window->draw(_wallSprites[Tiles::wallH]);

        _wallSprites[Tiles::wallH].setPosition(100+i*8, 100+21*8);
        _window->draw(_wallSprites[Tiles::wallH]);

        for (int j = 0; j < 20; ++j) {
          _wallSprites[Tiles::floorA].setPosition(100+i*8, 100+(j+1)*8);
          _window->draw(_wallSprites[Tiles::floorA]);
        }
      }

      for (int i = 0; i < 20; ++i) {
        _wallSprites[Tiles::wallV].setPosition(100, 100+(i+1)*8);
        _window->draw(_wallSprites[Tiles::wallV]);

        _wallSprites[Tiles::wallV].setPosition(100+21*8, 100+(i+1)*8);
        _window->draw(_wallSprites[Tiles::wallV]);
      }

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
