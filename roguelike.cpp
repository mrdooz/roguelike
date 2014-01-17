#include "game.hpp"
#include "utils.hpp"
#include "error.hpp"

using namespace rogue;

int main()
{
  if (!Game::Create())
    return 1;

  int res = GAME->Run();
  GAME->Close();
  return res;
}
