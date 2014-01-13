#include "debug_renderer.hpp"
#include "game.hpp"
#include "window_event_manager.hpp"
#include "animation_manager.hpp"
#include "virtual_window.hpp"
#include "shapes.hpp"

using namespace rogue;

//-----------------------------------------------------------------------------
AnimationWindow::AnimationWindow(
     const string& title,
     const Vector2f& pos,
     const Vector2f& size,
     DebugRenderer* renderer)
  : VirtualWindow(title, pos, size)
  , _renderer(renderer)
  , _curZoom(1)
{
}

//-----------------------------------------------------------------------------
bool AnimationWindow::Init()
{
  _windowManager->RegisterHandler(Event::KeyReleased, this, bind(&AnimationWindow::OnKeyReleased, this, _1));
  return true;
}


//-----------------------------------------------------------------------------
bool AnimationWindow::OnKeyReleased(const Event& event)
{
  Vector2f frameSize = _renderer->GetFrameSize();

  float maxScale = min((float)_size.x / frameSize.x, (float)_size.y / frameSize.y);

  if (event.key.code == Keyboard::Key::Add || event.key.code == Keyboard::Key::RBracket)
    _curZoom = min(maxScale, _curZoom + 1);

  if (event.key.code == Keyboard::Key::Subtract || event.key.code == Keyboard::Key::LBracket)
    _curZoom = max(1.0f, _curZoom - 1);

  return false;
}

//-----------------------------------------------------------------------------
void AnimationWindow::Draw()
{
  _texture.clear();

  // display the current animation

  ptime now = microsec_clock::local_time();
  if (_lastFrame.is_not_a_date_time())
    _lastFrame = now;

  time_duration delta = now - _lastFrame;


  // Fetch all animations
  vector<Animation*> animations;
  ANIMATION->GetAnimations(&animations);
  u32 curAnimation = Clamp(_renderer->_curAnimation, 0, (int)animations.size() - 1);
  _renderer->_curAnimation = curAnimation;

  Animation* animation = animations[curAnimation];
  size_t numFrames = animation->_frames.size();

  // Set the animation texture for the sprites
  _animationSprite.setTexture(animation->_texture);
  _animationFrames.resize(numFrames);

  // Draw header
  sf::Text text("", _font, 10);
  text.setString(toString("Animation: %d\n[%d frames, %d ms]",
                          curAnimation, numFrames, animation->_duration.total_milliseconds()));
  text.setPosition(sf::Vector2f(5, 5));
  text.setColor(sf::Color::White);
  _texture.draw(text);

  // Draw the frames
  // todo: split this over multiple lines in case of too many sprites
  Vector2f pos(10, 35);
  int maxHeight = 0;
  int maxWidth = 0;
  for (size_t i = 0; i < numFrames; ++i)
  {
    auto& frame = animation->_frames[i];
    auto& sprite = _animationFrames[i];
    sprite.setScale((float)_renderer->_curZoom, (float)_renderer->_curZoom);
    sprite.setTextureRect(frame._textureRect);
    maxHeight = max(maxHeight, frame._textureRect.height);
    maxWidth = max(maxWidth, frame._textureRect.width);
    sprite.setTexture(animation->_texture);
    sprite.setPosition(pos);
    pos.x += (i == numFrames - 1 ? 2 : 1) * frame._textureRect.width * _renderer->_curZoom;
    _texture.draw(sprite);
  }

  pos.x = 10;
  pos.y = 35 + (2 + maxHeight) * _renderer->_curZoom;

  // Draw the current frame
  float ratio = delta.total_milliseconds() / (float)animation->_duration.total_milliseconds();

  size_t frameIdx;
  size_t weightSum = animation->_weightSum;
  if (_renderer->_playOnce)
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
  _animationSprite.setScale(_curZoom, _curZoom);
  _texture.draw(_animationSprite);

  _texture.display();
}


//-----------------------------------------------------------------------------
CanvasWindow::CanvasWindow(
     const string& title,
     const Vector2f& pos,
     const Vector2f& size,
     DebugRenderer* renderer)
  : VirtualWindow(title, pos, size)
  , _renderer(renderer)
  , _gridSize(8, 8)
{
}

//-----------------------------------------------------------------------------
bool CanvasWindow::Init()
{
  if (!LoadImage("oryx_lofi/lofi_obj.png"))
    return false;

  _windowManager->RegisterHandler(Event::MouseMoved, this, bind(&CanvasWindow::OnMouseMove, this, _1));
  _windowManager->RegisterHandler(Event::KeyReleased, this, bind(&CanvasWindow::OnKeyReleased, this, _1));

  return true;
}

//-----------------------------------------------------------------------------
bool CanvasWindow::LoadImage(const char* filename)
{
  _editorImage.loadFromFile(filename);
  _imageSize = _editorImage.getSize();
  _imageDoubleBuffer.resize(_imageSize.x*_imageSize.y*4);
  const u8* ptr = _editorImage.getPixelsPtr();
  memcpy(_imageDoubleBuffer.data(), ptr, _imageSize.x*_imageSize.y*4);

  _editorTexture.loadFromImage(_editorImage);
  _editorSprite.setTexture(_editorTexture);

  UpdateDoubleBuffer();
  return true;
}

//-----------------------------------------------------------------------------
void CanvasWindow::UpdateDoubleBuffer()
{
  // copy the current animation/frame to the double buffer
  u32 curAnimation = _renderer->_curAnimation;
  u32 curFrame = _renderer->_curFrame;

  Frame frame;
  if (!ANIMATION->GetFrame(curAnimation, curFrame, &frame))
    return;

  size_t w = frame._textureRect.width;
  size_t h = frame._textureRect.height;

  size_t iw = _editorImage.getSize().x;

  _frameDoubleBuffer.resize(w * h * 4);

  u8* src = _imageDoubleBuffer.data() + (frame._textureRect.top * iw + frame._textureRect.left) * 4;
  u8* dst = _frameDoubleBuffer.data();

  for (size_t i = 0; i < h; ++i)
  {
    memcpy(&dst[i*w*4], &src[i*iw*4], w*4);
  }

  _frameTexture.create(w, h);
  _frameTexture.update(dst, w, h, 0, 0);

  _frameSprite.scale(_size.x / w, _size.y / h);
  _frameSprite.setTexture(_frameTexture, true);
}

//-----------------------------------------------------------------------------
Vector2f CanvasWindow::ImageOffset()
{
  float ix = (float)_imageSize.x;
  float iy = (float)_imageSize.y;

  float wx = _size.x;
  float wy = _size.y;

  View view(Vector2f(wx/2, wy/2), Vector2f(wx, wy));

  return Vector2f((wx - ix) / 2, (wy - iy) / 2);
}

//-----------------------------------------------------------------------------
bool CanvasWindow::OnKeyReleased(const Event& event)
{
  int prevFrame = _renderer->_curFrame;
  int numFrames = 5;

  if (event.key.code == Keyboard::Left)
    _renderer->_curFrame = Clamp(_renderer->_curFrame-1, 0, numFrames);

  if (event.key.code == Keyboard::Right)
    _renderer->_curFrame = Clamp(_renderer->_curFrame+1, 0, numFrames);

  if (prevFrame != _renderer->_curFrame)
    UpdateDoubleBuffer();

  return false;
}

//-----------------------------------------------------------------------------
bool CanvasWindow::OnMouseMove(const Event& event)
{
  /*
  sf::Vector2u size = _editorImage.getSize();
  Vector2f c = _texture.mapPixelToCoords(Vector2i(event.mouseMove.x, event.mouseMove.y)) - ImageOffset();
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
   */
  return true;
}

//-----------------------------------------------------------------------------
void CanvasWindow::Draw()
{
  _texture.clear();

  // Draw the current frame
//  _frameSprite.setScale(10, 10);

  // Draw the largest possible grid
  Vector2f frameSize = _renderer->GetFrameSize();

  u32 hLines = frameSize.x;
  u32 vLines = frameSize.y;

  float gx = _size.x / hLines;
  float gy = _size.y / vLines;

  _texture.draw(_frameSprite);

  float ix = (float)_size.x;
  float iy = (float)_size.y;

  VertexArray verts(sf::Lines, 2 * vLines + 2 * hLines);

  size_t idx = 0;
  for (size_t i = 0; i < vLines; ++i)
  {
    verts[idx+0].position = Vector2f(i*gx, 0);
    verts[idx+1].position = Vector2f(i*gx, iy);
    idx +=2;
  }

  for (size_t i = 0; i < hLines; ++i)
  {
    verts[idx+0].position = Vector2f(0, i*gy);
    verts[idx+1].position = Vector2f(ix, i*gy);
    idx +=2;
  }
  _texture.draw(verts);

  /*

  _texture.draw(_editorSprite);

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
  _texture.draw(verts);

  // Fetch all animations
  vector<Animation*> animations;
  ANIMATION->GetAnimations(&animations);
  _renderer->_curAnimation = Clamp(_renderer->_curAnimation, 0, (int)animations.size() - 1);

  Animation* animation = animations[_renderer->_curAnimation];
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

    _texture.draw(frameVerts);
  }
*/
  //_texture.draw(_sprCanvas);
  _texture.display();
}

//-----------------------------------------------------------------------------
ToolkitWindow::ToolkitWindow(const string& title, const Vector2f& pos, const Vector2f& size)
  : VirtualWindow(title, pos, size)
{

}

//-----------------------------------------------------------------------------
void ToolkitWindow::Draw()
{

}

//-----------------------------------------------------------------------------
FramesWindow::FramesWindow(const string& title, const Vector2f& pos, const Vector2f& size)
  : VirtualWindow(title, pos, size)
{

}

//-----------------------------------------------------------------------------
void FramesWindow::Draw()
{

}

//-----------------------------------------------------------------------------
ColorPickerWindow::ColorPickerWindow(const string& title, const Vector2f& pos, const Vector2f& size)
  : VirtualWindow(title, pos, size)
{

}

//-----------------------------------------------------------------------------
void ColorPickerWindow::Draw()
{

}

//-----------------------------------------------------------------------------
DebugRenderer::DebugRenderer(RenderWindow *window, WindowEventManager* eventManager)
  : _window(window)
  , _windowManager(window, eventManager)
  , _font(nullptr)
  , _curAnimation(0)
  , _curFrame(1)
  , _curZoom(3)
  , _playOnce(false)
  , _showGrid(true)
{
  eventManager->RegisterHandler(Event::Resized, bind(&DebugRenderer::OnResize, this, _1));
  eventManager->RegisterHandler(Event::KeyReleased, bind(&DebugRenderer::OnKeyReleased, this, _1));

  eventManager->RegisterHandler(Event::MouseButtonReleased, bind(&DebugRenderer::OnMouseButtonDown, this, _1));
  eventManager->RegisterHandler(Event::MouseMoved, bind(&DebugRenderer::OnMouseMove, this, _1));

  _windowManager.AddWindow(new AnimationWindow("ANIMATION", Vector2f(20, 20), Vector2f(200,200), this));
  _windowManager.AddWindow(new CanvasWindow("CANVAS", Vector2f(250,150), Vector2f(400,400), this));

  _windowManager.AddWindow(new ToolkitWindow("TOOLKIT", Vector2f(250,20), Vector2f(400,100)));
  _windowManager.AddWindow(new ColorPickerWindow("COLORPICKER", Vector2f(650,20), Vector2f(250,100)));
  _windowManager.AddWindow(new FramesWindow("FRAMES", Vector2f(650,150), Vector2f(250, 100)));
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
Vector2f DebugRenderer::GetFrameSize() const
{
  Vector2f res(0,0);

  vector<Animation*> animations;
  ANIMATION->GetAnimations(&animations);
  Animation* animation = animations[_curAnimation];

  for (const Frame& frame : animation->_frames)
  {
    res.x = max(res.x, (float)frame._textureRect.width);
    res.y = max(res.y, (float)frame._textureRect.height);
  }

  return res;
}


//-----------------------------------------------------------------------------
bool DebugRenderer::LoadImage(const char* filename)
{
/*
  _editorImage.loadFromFile(filename);
  _imageSize = _editorImage.getSize();
  _doubleBuffer.resize(_imageSize.x*_imageSize.y*4);
  const u8* ptr = _editorImage.getPixelsPtr();
  memcpy(_doubleBuffer.data(), ptr, _imageSize.x*_imageSize.y*4);

  _editorTexture.loadFromImage(_editorImage);
  _editorSprite.setTexture(_editorTexture);

  OnResize(Event());
*/
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
/*
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
*/
  return false;
}

//-----------------------------------------------------------------------------
Vector2i DebugRenderer::ImageOffset()
{
/*
  float ix = (float)_imageSize.x;
  float iy = (float)_imageSize.y;

  float wx = (float)_window->getSize().x;
  float wy = (float)_window->getSize().y;

  View view(Vector2f(wx/2, wy/2), Vector2f(wx, wy));

  return Vector2i((int)(wx - ix) / 2, (int)(wy - iy) / 2);
*/
  return Vector2i(0,0);
}

//-----------------------------------------------------------------------------
bool DebugRenderer::OnResize(const Event& event)
{
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
    //_lastFrame = boost::posix_time::not_a_date_time;
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
void DebugRenderer::Update()
{
  _windowManager.Update();
}
