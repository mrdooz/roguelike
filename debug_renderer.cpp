#include "debug_renderer.hpp"
#include "game.hpp"
#include "window_event_manager.hpp"

using namespace rogue;

//-----------------------------------------------------------------------------
DebugRenderer::DebugRenderer(RenderWindow *window)
  : _window(window)
  , _font(nullptr)
{
  DEBUG_WINDOW_EVENT->RegisterHandler(Event::Resized, bind(&DebugRenderer::OnResize, this, _1));
}

//-----------------------------------------------------------------------------
bool DebugRenderer::Init()
{
  _font = new sf::Font();
  if (!_font->loadFromFile("gfx/wscsnrg.ttf"))
    return false;

  return true;
}

//-----------------------------------------------------------------------------
bool DebugRenderer::OnResize(const Event& event)
{
  return false;
}

//-----------------------------------------------------------------------------
void DebugRenderer::Update()
{
  sf::Text normal("", *_font, 10);
  normal.setString("test");
  normal.setPosition(sf::Vector2f(100, 100));
  normal.setColor(sf::Color::White);
  _window->draw(normal);
}
