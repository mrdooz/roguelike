#include "precompiled.hpp"
#include "game.hpp"
#include "utils.hpp"
#include "roguelike.hpp"
#include "error.hpp"

using namespace rogue;

int main()
{
  if (!g_logSinkFile.Open("rogue.log"))
    return 1;

//  LOG_DEBUG(LogKeyValue("tjong", 10) << LogKeyValue("test", 10.0f));
//  s << LogKeyValue("tjong", 10) << LogKeyValue("test", 10.0f);
//  s << LogKeyValue("val", false);
//    << LogKeyValue("tjong2", 10.0f);

  srand(1337);

  if (!Game::create())
    return 1;

  int res = GAME.run();

  GAME.close();

  if (res)
  {
#ifdef _WIN32
    while (!isBitSet(GetAsyncKeyState(VK_ESCAPE), 15))
      ;
#endif
  }

  return res;
}
