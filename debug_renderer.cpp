#include "precompiled.hpp"
#include "roguelike.hpp"
#include "debug_renderer.hpp"

using namespace rogue;

DebugRenderer::DebugRenderer(RenderWindow *window)
  : _window(window)
  , _font(nullptr)
{
}

bool DebugRenderer::Init()
{
  _font = new sf::Font();
  if (!_font->loadFromFile("gfx/wscsnrg.ttf"))
    return false;

  return true;
}

void DebugRenderer::Resize()
{

}

void DebugRenderer::Update(const GameState& state, const sf::Event* event)
{
  sf::Text normal("", *_font, 10);
  normal.setString("test");
  normal.setPosition(sf::Vector2f(100, 100));
  normal.setColor(sf::Color::White);
  _window->draw(normal);
}
