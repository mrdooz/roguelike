#include "debug_renderer.hpp"
#include "game.hpp"
#include "window_event_manager.hpp"
#include "animation_manager.hpp"
#include "virtual_window.hpp"
#include "shapes.hpp"

using namespace rogue;

struct AnimationWindow : public VirtualWindow
{
  AnimationWindow(const string& title, const Vector2f& pos, const Vector2f& size)
    : VirtualWindow(title, pos, size)
  {
  }

  void Draw()
  {
    _texture.clear();

    //sf::RoundedRectangleShape rect(VectorCast<float>(_size), 100, 100);
    //_texture.draw(rect);
    _texture.display();
  }

  Font _font;
};

struct CanvasWindow : public VirtualWindow
{
  CanvasWindow(const string& title, const Vector2f& pos, const Vector2f& size)
    : VirtualWindow(title, pos, size)
  {
  }

  void Draw()
  {
    _texture.clear();

    sf::RectangleShape rect(VectorCast<float>(_size));
    rect.setFillColor(_focus ? Color::White : Color::Yellow);
    _texture.draw(rect);
    _texture.display();
  }
};

struct ColorPickerWindow : public VirtualWindow
{

};

struct FramesWindow : public VirtualWindow
{

};

//-----------------------------------------------------------------------------
DebugRenderer::DebugRenderer(RenderWindow *window, WindowEventManager* eventManager)
  : _window(window)
  , _windowManager(window, eventManager)
  , _font(nullptr)
  , _gridSize(8, 8)
  , _curAnimation(0)
  , _curZoom(3)
  , _playOnce(false)
  , _showGrid(true)
{
  eventManager->RegisterHandler(Event::Resized, bind(&DebugRenderer::OnResize, this, _1));
  eventManager->RegisterHandler(Event::KeyReleased, bind(&DebugRenderer::OnKeyReleased, this, _1));

  eventManager->RegisterHandler(Event::MouseButtonReleased, bind(&DebugRenderer::OnMouseButtonDown, this, _1));
  eventManager->RegisterHandler(Event::MouseMoved, bind(&DebugRenderer::OnMouseMove, this, _1));

  _windowManager.AddWindow(new AnimationWindow("ANIMATION", Vector2f(20, 20), Vector2f(200,200)));
  _windowManager.AddWindow(new CanvasWindow("CANVAS", Vector2f(250,20), Vector2f(500,500)));
}

//-----------------------------------------------------------------------------
bool DebugRenderer::Init()
{
  _font = new sf::Font();
  if (!_font->loadFromFile("gfx/wscsnrg.ttf"))
    return false;

  if (!LoadImage("oryx_lofi/lofi_obj.png"))
    return false;

  _windowManager.Init();

  return true;
}

//-----------------------------------------------------------------------------
bool DebugRenderer::LoadImage(const char* filename)
{
  _editorImage.loadFromFile(filename);
  _imageSize = _editorImage.getSize();
  _doubleBuffer.resize(_imageSize.x*_imageSize.y*4);
  const u8* ptr = _editorImage.getPixelsPtr();
  memcpy(_doubleBuffer.data(), ptr, _imageSize.x*_imageSize.y*4);

  _editorTexture.loadFromImage(_editorImage);
  _editorSprite.setTexture(_editorTexture);

  OnResize(Event());

  return true;
}

//-----------------------------------------------------------------------------
bool DebugRenderer::OnMouseButtonDown(const Event& event)
{
  return false;
}

//-----------------------------------------------------------------------------
bool DebugRenderer::OnMouseMove(const Event& event)
{
  sf::Vector2u size = _editorImage.getSize();
  Vector2f c = _window->mapPixelToCoords(Vector2i(event.mouseMove.x, event.mouseMove.y)) - VectorCast<float>(ImageOffset());
  int x = (int)c.x;
  int y = (int)c.y;

  if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
    return false;

  if (x < 0 || x >= (int)size.x || y < 0 || y >= (int)size.y)
    return false;

  _doubleBuffer[(y*size.x+x)*4+0] = 0xff;
  _doubleBuffer[(y*size.x+x)*4+1] = 0xff;
  _doubleBuffer[(y*size.x+x)*4+2] = 0xff;
  _doubleBuffer[(y*size.x+x)*4+3] = 0xff;

  _editorTexture.update(_doubleBuffer.data(), size.x, size.y, 0, 0);

  return false;
}

//-----------------------------------------------------------------------------
Vector2i DebugRenderer::ImageOffset()
{
  float ix = (float)_imageSize.x;
  float iy = (float)_imageSize.y;

  float wx = (float)_window->getSize().x;
  float wy = (float)_window->getSize().y;

  View view(Vector2f(wx/2, wy/2), Vector2f(wx, wy));

  return Vector2i((int)(wx - ix) / 2, (int)(wy - iy) / 2);
}

//-----------------------------------------------------------------------------
void DebugRenderer::DrawAnimationWidget()
{
  _rtAnimation.clear();

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
  sf::Text text("", *_font, 15);
  text.setString(toString("Animation: %d [%d frames, %d ms]",
      _curAnimation, numFrames, animation->_duration.total_milliseconds()));
  text.setPosition(sf::Vector2f(5, 5));
  text.setColor(sf::Color::White);
  _rtAnimation.draw(text);

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
    _rtAnimation.draw(sprite);
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
  _rtAnimation.draw(_animationSprite);

  _rtAnimation.display();

  _window->draw(_sprAnimation);
}

//-----------------------------------------------------------------------------
void DebugRenderer::DrawCanvasWidget()
{
  _rtCanvas.clear();

  _rtCanvas.draw(_editorSprite);

  float ix = (float)_imageSize.x;
  float iy = (float)_imageSize.y;

  float gx = (float)_gridSize.x;
  float gy = (float)_gridSize.y;

  Vector2f ofs(0,0); // (VectorCast<float>(ImageOffset()));

  size_t vLines = _imageSize.x / _gridSize.x;
  size_t hLines = _imageSize.y / _gridSize.y;

  VertexArray verts(sf::Lines, 2 * vLines + 2 * hLines);

  size_t idx = 0;
  for (size_t i = 0; i < vLines; ++i)
  {
    verts[idx+0].position = ofs + Vector2f(i*gx, 0);
    verts[idx+1].position = ofs + Vector2f(i*gx, iy);
    idx +=2;
  }

  for (size_t i = 0; i < hLines; ++i)
  {
    verts[idx+0].position = ofs + Vector2f(0, i*gy);
    verts[idx+1].position = ofs + Vector2f(ix, i*gy);
    idx +=2;
  }
  _rtCanvas.draw(verts);

  // Fetch all animations
  vector<Animation*> animations;
  ANIMATION->GetAnimations(&animations);
  _curAnimation = Clamp(_curAnimation, 0, (int)animations.size() - 1);

  Animation* animation = animations[_curAnimation];
  size_t numFrames = animation->_frames.size();

  // Draw the frames
  for (size_t i = 0; i < numFrames; ++i)
  {
    VertexArray frameVerts(sf::LinesStrip, 4);

    auto& frame = animation->_frames[i];

    // convert texture rect to coords
    auto& r = frame._textureRect;
    float left    = (float)r.left;
    float top     = (float)r.top;
    float width   = (float)r.width;
    float height  = (float)r.height;

    frameVerts[0].position = ofs + Vector2f(left, top);
    frameVerts[0].color = Color::Red;
    frameVerts[1].position = frameVerts[0].position + Vector2f(width, 0);
    frameVerts[1].color = Color::Red;
    frameVerts[2].position = frameVerts[1].position + Vector2f(0, height);
    frameVerts[2].color = Color::Red;
    frameVerts[3].position = frameVerts[0].position + Vector2f(0, height);
    frameVerts[3].color = Color::Red;

    _rtCanvas.draw(frameVerts);
  }

  _rtCanvas.display();

  _window->draw(_sprCanvas);
}

//-----------------------------------------------------------------------------
void DebugRenderer::DrawColorPickerWidget()
{
  _rtColorPicker.clear();
  RectangleShape rs;
  rs.setSize(Vector2f(200, 200));
  rs.setFillColor(sf::Color::Blue);
  _rtColorPicker.draw(rs);
  _rtColorPicker.display();

  _window->draw(_sprColorPicker);
}

//-----------------------------------------------------------------------------
void DebugRenderer::DrawFramesWidget()
{
  _rtFrames.clear();
  RectangleShape rs;
  rs.setSize(Vector2f(200, 200));
  rs.setFillColor(sf::Color::Green);
  _rtFrames.draw(rs);
  _rtFrames.display();

  _window->draw(_sprFrames);
}

//-----------------------------------------------------------------------------
bool DebugRenderer::OnResize(const Event& event)
{
  float wx = (float)_window->getSize().x;
  float wy = (float)_window->getSize().y;
  View view(Vector2f(wx/2, wy/2), Vector2f(wx, wy));

  // center the sprite on the window
  //_editorSprite.setPosition(VectorCast<float>(ImageOffset()));
  //view.zoom(1/(float)_curZoom);

  _window->setView(view);

  // Setup the various render targets
  _rtAnimation.create(300, 300);
  _rtCanvas.create(500, 500);
  _rtColorPicker.create(200, 200);
  _rtFrames.create(200, 400);

  _sprAnimation.setTexture(_rtAnimation.getTexture());
  _sprAnimation.setPosition(0,0);

  _sprCanvas.setTexture(_rtCanvas.getTexture());
  _sprCanvas.setPosition(300, 0);

  _sprColorPicker.setTexture(_rtColorPicker.getTexture());
  _sprColorPicker.setPosition(800, 0);

  _sprFrames.setTexture(_rtFrames.getTexture());
  _sprFrames.setPosition(800, 200);

  return false;
}

//-----------------------------------------------------------------------------
bool DebugRenderer::OnKeyReleased(const Event& event)
{
  int _oldZoom = _curZoom;

  if (event.key.code == Keyboard::Key::G)
    _showGrid = !_showGrid;

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

  if (_oldZoom != _curZoom)
  {
    // todo: fix this. zoom is relative, and not absolute
    //auto view = _window->getView();
    //view.zoom(1 / (float)_curZoom);
    //_window->setView(view);
  }

  return false;
}

//-----------------------------------------------------------------------------
void DebugRenderer::DrawEditor()
{
/*
  _window->draw(_editorSprite);

  if (_showGrid)
    DrawGrid();
*/
  DrawAnimationWidget();
  DrawCanvasWidget();
  DrawColorPickerWidget();
  DrawFramesWidget();
}

//-----------------------------------------------------------------------------
void DebugRenderer::DrawGrid()
{
  float ix = (float)_imageSize.x;
  float iy = (float)_imageSize.y;

  float gx = (float)_gridSize.x;
  float gy = (float)_gridSize.y;

  Vector2f ofs(VectorCast<float>(ImageOffset()));

  size_t vLines = _imageSize.x / _gridSize.x;
  size_t hLines = _imageSize.y / _gridSize.y;

  VertexArray verts(sf::Lines, 2 * vLines + 2 * hLines);

  size_t idx = 0;
  for (size_t i = 0; i < vLines; ++i)
  {
    verts[idx+0].position = ofs + Vector2f(i*gx, 0);
    verts[idx+1].position = ofs + Vector2f(i*gx, iy);
    idx +=2;
  }

  for (size_t i = 0; i < hLines; ++i)
  {
    verts[idx+0].position = ofs + Vector2f(0, i*gy);
    verts[idx+1].position = ofs + Vector2f(ix, i*gy);
    idx +=2;
  }
  _window->draw(verts);

  // Fetch all animations
  vector<Animation*> animations;
  ANIMATION->GetAnimations(&animations);
  _curAnimation = Clamp(_curAnimation, 0, (int)animations.size() - 1);

  Animation* animation = animations[_curAnimation];
  size_t numFrames = animation->_frames.size();

  // Draw the frames
  for (size_t i = 0; i < numFrames; ++i)
  {
    VertexArray frameVerts(sf::LinesStrip, 4);

    auto& frame = animation->_frames[i];

    // convert texture rect to coords
    auto& r = frame._textureRect;
    float left    = (float)r.left;
    float top     = (float)r.top;
    float width   = (float)r.width;
    float height  = (float)r.height;

    frameVerts[0].position = ofs + Vector2f(left, top);
    frameVerts[0].color = Color::Red;
    frameVerts[1].position = frameVerts[0].position + Vector2f(width, 0);
    frameVerts[1].color = Color::Red;
    frameVerts[2].position = frameVerts[1].position + Vector2f(0, height);
    frameVerts[2].color = Color::Red;
    frameVerts[3].position = frameVerts[0].position + Vector2f(0, height);
    frameVerts[3].color = Color::Red;

    _window->draw(frameVerts);
  }
 
}

//-----------------------------------------------------------------------------
void DebugRenderer::Update()
{
  _windowManager.Update();
  //DrawEditor();
}
