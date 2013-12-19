#ifndef _LEVEL_FACTORY_HPP_
#define _LEVEL_FACTORY_HPP_

#include "level.hpp"

namespace rogue
{
  class LevelFactory {

  public:
    static bool create();
    static void close();
    static LevelFactory &instance();

    Level *CreateLevel(int width, int height);
  private:

    static LevelFactory *_instance;
  };
}


#endif
