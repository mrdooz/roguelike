#include "precompiled.hpp"
#include "game.hpp"
#include "utils.hpp"
#include "roguelike.hpp"

using namespace rogue;

int main() {
  srand(1337);

  if (!Game::create())
    return 1;

  int res = GAME.run();

  GAME.close();

  if (res) {
#ifdef _WIN32
    while (!isBitSet(GetAsyncKeyState(VK_ESCAPE), 15))
      ;
#endif
  }

  return res;
}
