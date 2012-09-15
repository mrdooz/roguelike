#include "stdafx.h"
#include "utils.hpp"

using namespace std;

class App {
public:

  bool init() {
    findAppRoot();
    return true;
  }

  int run()
  {
    // Create the main window
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML window");

    // Load a sprite to display
    sf::Texture texture;
    if (!texture.loadFromFile("oryx_lofi\\preview_sample.png"))
      return EXIT_FAILURE;
    sf::Sprite sprite(texture);

    // Create a graphical text to display
    sf::Font font;
    if (!font.loadFromFile("gfx\\wscsnrg.ttf"))
      return EXIT_FAILURE;
    sf::Text text("Hello SFML", font, 20);

    // Start the game loop
    while (window.isOpen())
    {
      // Process events
      sf::Event event;
      while (window.pollEvent(event))
      {
        // Close window : exit
        if (event.type == sf::Event::Closed)
          window.close();
      }

      // Clear screen
      window.clear();

      // Draw the sprite
      window.draw(sprite);

      // Draw the string
      window.draw(text);

      // Update the window
      window.display();
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

  if (res) {
    while (!isBitSet(GetAsyncKeyState(VK_ESCAPE), 15))
      ;
  }

  return res;
}
