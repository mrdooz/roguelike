#include "precompiled.hpp"
#include "game.hpp"
#include "utils.hpp"

int main() {
  srand(1337);

  Game game;
  if (!game.init())
    return 1;

  int res = game.run();

  game.close();

  if (res) {
#ifdef _WIN32
    while (!isBitSet(GetAsyncKeyState(VK_ESCAPE), 15))
      ;
#endif
  }

  return res;
}
