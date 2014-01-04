#include "debug_renderer.hpp"
#include "game.hpp"
#include "window_event_manager.hpp"
#include "animation_manager.hpp"

using namespace rogue;

//-----------------------------------------------------------------------------
DebugRenderer::DebugRenderer(RenderWindow *window)
  : _window(window)
  , _font(nullptr)
  , _curAnimation(0)
  , _curZoom(3)
  , _playOnce(false)
{
  DEBUG_WINDOW_EVENT->RegisterHandler(Event::Resized, bind(&DebugRenderer::OnResize, this, _1));
  DEBUG_WINDOW_EVENT->RegisterHandler(Event::KeyReleased, bind(&DebugRenderer::OnKeyReleased, this, _1));
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
bool DebugRenderer::OnKeyReleased(const Event& event)
{
  if (event.key.code == Keyboard::Key::Up)
    _curAnimation++;

  if (event.key.code == Keyboard::Key::Down)
    _curAnimation = max(0, _curAnimation - 1);

  if (event.key.code == Keyboard::Key::Add)
    _curZoom = min(10, _curZoom + 1);

  if (event.key.code == Keyboard::Key::Subtract)
    _curZoom = max(1, _curZoom - 1);

  if (event.key.code == Keyboard::Key::Space)
    _playOnce = false;

  if (event.key.code == Keyboard::Key::Return)
  {
    _lastFrame = boost::posix_time::not_a_date_time;
    _playOnce = true;
  }

  return false;
}

//-----------------------------------------------------------------------------
void DebugRenderer::Update()
{
  ptime now = microsec_clock::local_time();
  if (_lastFrame.is_not_a_date_time())
    _lastFrame = now;

  time_duration delta = now - _lastFrame;

  // Fetch all animations
  vector<Animation*> animations;
  ANIMATION->GetAnimations(&animations);
  _curAnimation = Clamp(_curAnimation, 0, (int)animations.size() - 1);

  Animation* animation = animations[_curAnimation];
  size_t numFrames = animation->_frames.size();

  // Set the animation texture for the sprites
  _animationSprite.setTexture(animation->_texture);
  _animationFrames.resize(numFrames);

  // Draw header
  sf::Text normal("", *_font, 15);
  normal.setString(toString("Animation: %d [%d frames, %d ms]",
      _curAnimation, numFrames, animation->_duration.total_milliseconds()));
  normal.setPosition(sf::Vector2f(5, 5));
  normal.setColor(sf::Color::White);
  _window->draw(normal);

  // Draw the frames
  Vector2f pos(10, 25);
  for (size_t i = 0; i < numFrames; ++i)
  {
    auto& frame = animation->_frames[i];
    auto& sprite = _animationFrames[i];
    sprite.setScale((float)_curZoom, (float)_curZoom);
    sprite.setTextureRect(frame._textureRect);
    sprite.setTexture(animation->_texture);
    sprite.setPosition(pos);
    pos.x += (i == numFrames - 1 ? 2 : 1) * frame._textureRect.width * _curZoom;
    _window->draw(sprite);
  }

  // Draw the current frame
  float ratio = delta.total_milliseconds() / (float)animation->_duration.total_milliseconds();

  size_t frameIdx;
  size_t weightSum = animation->_weightSum;
  if (_playOnce)
  {
    frameIdx = min((size_t)(ratio * weightSum), weightSum - 1);
  }
  else
  {
    frameIdx = (size_t)(ratio * weightSum) % weightSum;
  }

  // apply frame weights
  frameIdx = animation->_frameIndex[frameIdx];

  _animationSprite.setTextureRect(animation->_frames[frameIdx]._textureRect);
  _animationSprite.setPosition(pos);
  _animationSprite.setScale((float)_curZoom, (float)_curZoom);
  _window->draw(_animationSprite);

}
