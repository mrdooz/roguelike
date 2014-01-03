#pragma once

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
  using std::deque;
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

  using boost::posix_time::ptime;
  using boost::posix_time::time_duration;
  using boost::posix_time::microsec_clock;
  using boost::posix_time::seconds;

  using sf::Color;
  using sf::Font;
  using sf::Texture;
  using sf::Sprite;
  using sf::RectangleShape;
  using sf::RenderTarget;
  using sf::RenderWindow;
  using sf::RenderTexture;
  using sf::Keyboard;
  using sf::Event;
  using sf::View;

  using sf::Vector2f;
  using sf::Vector2i;

  typedef sf::Vector2i Pos;
  typedef sf::Rect<int> Rect;
}
