#ifndef _LEVEL_FACTORY_HPP_
#define _LEVEL_FACTORY_HPP_

#include "level.hpp"

class LevelFactory {

public:
  static bool create();
  static void close();
  static LevelFactory &instance();

  Level *createLevel(int width, int height, const sf::Texture &envTexture, const sf::Texture &charTexture);
private:

  //void subdivide(Level *level, int top, int left, int width, int height, int depth);

  static LevelFactory *_instance;
};

#endif
