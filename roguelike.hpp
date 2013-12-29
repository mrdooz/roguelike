#pragma once

#include <memory>
#include <functional>
#include <vector>
#include <string>
#include <set>
#include <unordered_map>
#include <queue>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

namespace rogue
{
  using std::bind;
  using std::function;
  using std::placeholders::_1;
  using std::placeholders::_2;
  using std::placeholders::_3;
  using std::vector;
  using std::string;
  using std::set;
  using std::priority_queue;
  using std::map;
  using std::unordered_multimap;
  using std::unordered_map;
  using std::pair;
  using std::make_pair;
  using std::shared_ptr;
  using std::unique_ptr;

  using std::function;
  
  using std::min;
  using std::max;


  using sf::Font;
  using sf::Texture;
  using sf::Sprite;
  using sf::RenderWindow;
  using sf::Keyboard;
  using sf::Event;

  using sf::Vector2f;

}