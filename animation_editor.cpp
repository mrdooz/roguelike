#include "animation_editor.hpp"
#include "game.hpp"
#include "window_event_manager.hpp"
#include "animation_manager.hpp"
#include "virtual_window.hpp"
#include "sfml_helpers.hpp"

using namespace rogue;
using namespace sf;

//-----------------------------------------------------------------------------
AnimationWindow::AnimationWindow(
     const string& title,
     const Vector2f& pos,
     const Vector2f& size,
     AnimationEditor* renderer)
  : VirtualWindow(title, pos, size)
  , _editor(renderer)
  , _curZoom(1)
{
}

//-----------------------------------------------------------------------------
bool AnimationWindow::Init()
{
  if (!VirtualWindow::Init())
    return false;

  _windowManager->RegisterHandler(Event::KeyReleased, this, bind(&AnimationWindow::OnKeyReleased, this, _1));
  return true;
}

//-----------------------------------------------------------------------------
bool AnimationWindow::OnKeyReleased(const Event& event)
{
  Vector2f frameSize = _editor->GetFrameSize();

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
  u32 curAnimation = Clamp(_editor->_curAnimation, 0, (int)animations.size() - 1);
  _editor->_curAnimation = curAnimation;

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
    sprite.setScale((float)_editor->_curZoom, (float)_editor->_curZoom);
    sprite.setTextureRect(frame._textureRect);
    maxHeight = max(maxHeight, frame._textureRect.height);
    maxWidth = max(maxWidth, frame._textureRect.width);
    sprite.setTexture(animation->_texture);
    sprite.setPosition(pos);
    pos.x += (i == numFrames - 1 ? 2 : 1) * frame._textureRect.width * _editor->_curZoom;
    _texture.draw(sprite);
  }

  pos.x = 10;
  pos.y = 35 + (2.0f + maxHeight) * _editor->_curZoom;

  // Draw the current frame
  float ratio = delta.total_milliseconds() / (float)animation->_duration.total_milliseconds();

  size_t frameIdx;
  size_t weightSum = animation->_weightSum;
  if (_editor->_playOnce)
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
     AnimationEditor* renderer)
  : VirtualWindow(title, pos, size)
  , _editor(renderer)
  , _gridSize(8, 8)
  , _showGrid(true)
{
}

//-----------------------------------------------------------------------------
bool CanvasWindow::Init()
{
  if (!VirtualWindow::Init())
    return false;

  if (!LoadImage("oryx_lofi/lofi_obj.png"))
    return false;

  _windowManager->RegisterHandler(Event::MouseMoved, this, bind(&CanvasWindow::OnMouseMove, this, _1));
  _windowManager->RegisterHandler(Event::MouseButtonPressed, this, bind(&CanvasWindow::OnMouseButtonPressed, this, _1));
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
  u32 curAnimation = _editor->_curAnimation;
  u32 curFrame = _editor->_curFrame;

  Frame frame;
  if (!ANIMATION->GetFrame(curAnimation, curFrame, &frame))
    return;

  size_t w = frame._textureRect.width;
  size_t h = frame._textureRect.height;

  _frameSize.x = w;
  _frameSize.y = h;

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

  _scale = min(_size.x / w, _size.y / h);

  _frameSprite.setScale(_scale, _scale);
  _frameSprite.setTexture(_frameTexture, true);
}

//-----------------------------------------------------------------------------
bool CanvasWindow::OnKeyReleased(const Event& event)
{
  int prevFrame = _editor->_curFrame;
  int numFrames = 5;

  if (event.key.code == Keyboard::Left)
    _editor->_curFrame = Clamp(_editor->_curFrame-1, 0, numFrames);

  if (event.key.code == Keyboard::Right)
    _editor->_curFrame = Clamp(_editor->_curFrame+1, 0, numFrames);

  if (event.key.code == Keyboard::Key::G)
    _showGrid = !_showGrid;

  if (prevFrame != _editor->_curFrame)
    UpdateDoubleBuffer();

  return false;
}

//-----------------------------------------------------------------------------
bool CanvasWindow::OnMouseButtonPressed(const Event& event)
{
  int x = event.mouseButton.x / _scale;
  int y = event.mouseButton.y / _scale;
  UpdateFrameBuffer(x, y, event.mouseButton.button);
  return true;
}

//-----------------------------------------------------------------------------
bool CanvasWindow::OnMouseMove(const Event& event)
{
  int x = event.mouseButton.x / _scale;
  int y = event.mouseButton.y / _scale;

  Mouse::Button btn = Mouse::Button::ButtonCount;
  if (Mouse::isButtonPressed(Mouse::Button::Left))
    btn = Mouse::Button::Left;

  if (Mouse::isButtonPressed(Mouse::Button::Right))
    btn = Mouse::Button::Right;

  UpdateFrameBuffer(x, y, btn);
  return true;
}

//-----------------------------------------------------------------------------
void CanvasWindow::UpdateFrameBuffer(int x, int y, Mouse::Button btn)
{
  if (!(btn == Mouse::Button::Left || btn == Mouse::Button::Right) )
    return;

  int w = _frameSize.x;
  int h = _frameSize.y;

  if (x < 0 || x >= w || y < 0 || y >= h)
    return;

  if (_editor->_eyeDropper)
  {
    // If in color picker mode, set the primary color to the color
    // under the cursor
    u32* src = (u32*)&_frameDoubleBuffer[(y*w+x)*4+0];
    _editor->_primaryColor = HsvFromColor(ColorFromU32(*src));
    _editor->_eyeDropper = false;
  }
  else
  {
    Color color(btn == Mouse::Button::Left ? _editor->_primaryColor : _editor->_secondaryColor);

    _frameDoubleBuffer[(y*w+x)*4+0] = color.r;  // r
    _frameDoubleBuffer[(y*w+x)*4+1] = color.g;  // g
    _frameDoubleBuffer[(y*w+x)*4+2] = color.b;  // b
    _frameDoubleBuffer[(y*w+x)*4+3] = color.a;  // a

    _frameTexture.update(_frameDoubleBuffer.data(), w, h, 0, 0);
  }
}

//-----------------------------------------------------------------------------
void CanvasWindow::Draw()
{
  _texture.clear();

  // Draw the current frame
  _texture.draw(_frameSprite);

  // Draw the largest possible grid
  if (_showGrid)
  {
    Vector2f frameSize = _editor->GetFrameSize();

    u32 hLines = frameSize.x;
    u32 vLines = frameSize.y;

    float gx = _size.x / hLines;
    float gy = _size.y / vLines;

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
  }

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
ColorPickerWindow::ColorPickerWindow(
    const string& title,
    const Vector2f& pos,
    const Vector2f& size,
    AnimationEditor* editor)
  : VirtualWindow(title, pos, size)
  , _editor(editor)
  , _valueWidth(75)
  , _swatchSize(60)
{
}

//-----------------------------------------------------------------------------
bool ColorPickerWindow::Init()
{
  if (!VirtualWindow::Init())
    return false;

  _windowManager->RegisterHandler(Event::Resized, this, bind(&ColorPickerWindow::OnResized, this, _1));

  _windowManager->RegisterHandler(Event::MouseMoved, this, bind(&ColorPickerWindow::OnMouseMoved, this, _1));
  _windowManager->RegisterHandler(Event::MouseButtonPressed, this, bind(&ColorPickerWindow::OnMousePressed, this, _1));

  _windowManager->RegisterHandler(Event::KeyReleased, this, bind(&ColorPickerWindow::OnKeyReleased, this, _1));

  OnResized(Event());

  return true;
}

//-----------------------------------------------------------------------------
void ColorPickerWindow::DrawSliders()
{
  _texture.draw(_pickerHSprite);
  _texture.draw(_pickerSSprite);
  _texture.draw(_pickerVSprite);

  Text text("", _font, 10);

  float hue = _editor->_primaryColor.h;
  float saturation = _editor->_primaryColor.s;
  float value = _editor->_primaryColor.v;

  // draw sliders and values
  float w = _size.x - _valueWidth;
  float h = _size.y - _swatchSize;
  VertexArray verts(Lines, 6);
  float y = 0;
  verts[0].position = Vector2f(w*hue, y);
  verts[1].position = Vector2f(w*hue, y + h/3);
  text.setString(toString("H: %.2f", hue * 360));
  text.setPosition(_size.x - _valueWidth + 5, y + 15);
  _texture.draw(text);

  y += h/3;
  verts[2].position = Vector2f(w*saturation, y);
  verts[3].position = Vector2f(w*saturation, y + h/3);
  text.setString(toString("S: %.2f", saturation));
  text.setPosition(_size.x - _valueWidth + 5, y + 15);
  _texture.draw(text);

  y += h/3;
  verts[4].position = Vector2f(w*value, y);
  verts[5].position = Vector2f(w*value, y + h/3);
  text.setString(toString("V: %.2f", value));
  text.setPosition(_size.x - _valueWidth + 5, y + 15);
  _texture.draw(text);
  _texture.draw(verts);

}

//-----------------------------------------------------------------------------
void ColorPickerWindow::DrawSwatches()
{
  int h = _size.y - _swatchSize;

  RectangleShape primary;
  primary.setFillColor(_editor->_primaryColor);
  primary.setOutlineColor(Color::White);
  primary.setOutlineThickness(-2);
  primary.setPosition(0, h);
  primary.setSize(Vector2f(_swatchSize, _swatchSize));
  _texture.draw(primary);

  RectangleShape secondary;
  secondary.setFillColor(_editor->_secondaryColor);
  secondary.setOutlineColor(Color::White);
  secondary.setOutlineThickness(-2);
  secondary.setPosition(_swatchSize, h);
  secondary.setSize(Vector2f(_swatchSize, _swatchSize));
  _texture.draw(secondary);

  // draw the swatches in the remaining space
  int s = (_size.x - 2 * _swatchSize) / _editor->_swatch.size();
  Vector2f pos(2*_swatchSize, _size.y - s);
  RectangleShape r;
  r.setOutlineColor(Color::White);
  r.setOutlineThickness(-1);
  r.setSize(Vector2f(s, s));
  for (const HsvColor& c : _editor->_swatch)
  {
    r.setFillColor(c);
    r.setPosition(pos);
    _texture.draw(r);
    pos.x += s;
  }
}

//-----------------------------------------------------------------------------
void ColorPickerWindow::Draw()
{
  _texture.clear();
  DrawSliders();
  DrawSwatches();
  _texture.display();
}

//-----------------------------------------------------------------------------
void ColorPickerWindow::UpdatePicker()
{
  float hue = _editor->_primaryColor.h;
  float saturation = _editor->_primaryColor.s;
  float value = _editor->_primaryColor.v;

  int w = _size.x - _valueWidth;
  vector<u32> tmp(w);

  // update hue
  for (int i = 0; i < w; ++i)
  {
    tmp[i] = ColorToU32(ColorFromHsv(360.0f/(w-1) * i, saturation, value));
  }
  _pickerHTexture.update((const u8*)tmp.data(), w, 1, 0, 0);

  // update saturation
  for (int i = 0; i < w; ++i)
  {
    tmp[i] = ColorToU32(ColorFromHsv(hue*360, (float)i/(w-1), value));
  }
  _pickerSTexture.update((const u8*)tmp.data(), w, 1, 0, 0);

  // update value (brightness)
  for (int i = 0; i < w; ++i)
  {
    tmp[i] = ColorToU32(ColorFromHsv(hue*360, saturation, (float)i/(w-1)));
  }
  _pickerVTexture.update((const u8*)tmp.data(), w, 1, 0, 0);
}

//-----------------------------------------------------------------------------
void ColorPickerWindow::UpdateHsv(int x, int y)
{
  float w = _size.x - _valueWidth;
  float h = (_size.y - _swatchSize) / 3;
  float v = Clamp(x / w, 0.f, 1.f);
  Vector2i pos(x, y);

  if (Rect(0, 0*h, w*1.05f, h).contains(pos))
  {
    _editor->_primaryColor.h = v;
  }
  else if (Rect(0, 1*h, w*1.05f, h).contains(pos))
  {
    _editor->_primaryColor.s = v;
  }
  else if (Rect(0, 2*h, w*1.05f, h).contains(pos))
  {
    _editor->_primaryColor.v = v;
  }

  UpdatePicker();
}

//-----------------------------------------------------------------------------
bool ColorPickerWindow::OnKeyReleased(const Event& event)
{
  return false;
}

//-----------------------------------------------------------------------------
bool ColorPickerWindow::OnMousePressed(const Event& event)
{
  if (event.mouseButton.button != Mouse::Left)
    return false;

  UpdateHsv(event.mouseButton.x, event.mouseButton.y);
  return false;
}

//-----------------------------------------------------------------------------
bool ColorPickerWindow::OnMouseMoved(const Event& event)
{
  if (!Mouse::isButtonPressed(Mouse::Left))
    return false;

  UpdateHsv(event.mouseMove.x, event.mouseMove.y);
  return false;
}

//-----------------------------------------------------------------------------
bool ColorPickerWindow::OnResized(const Event& event)
{
  float h = (_size.y - _swatchSize) / 3;

  _pickerHTexture.create(_size.x - _valueWidth, 1);
  _pickerHSprite.setTexture(_pickerHTexture, true);
  _pickerHSprite.setScale(1, h);
  _pickerHSprite.setPosition(0, 0);

  _pickerSTexture.create(_size.x - _valueWidth, 1);
  _pickerSSprite.setTexture(_pickerSTexture, true);
  _pickerSSprite.setScale(1, h);
  _pickerSSprite.setPosition(0, h);

  _pickerVTexture.create(_size.x - _valueWidth, 1);
  _pickerVSprite.setTexture(_pickerVTexture, true);
  _pickerVSprite.setScale(1, h);
  _pickerVSprite.setPosition(0, 2*h);

  UpdatePicker();

  return true;
}

//-----------------------------------------------------------------------------
AnimationEditor::AnimationEditor(RenderWindow *window, WindowEventManager* eventManager)
  : _window(window)
  , _windowManager(window, eventManager)
  , _font(nullptr)
  , _curAnimation(0)
  , _curFrame(1)
  , _curZoom(3)
  , _playOnce(false)
  , _primaryColor(360, 1, 1)
  , _secondaryColor(0, 0, 0)
  , _swatch(10)
  , _eyeDropper(false)
{
  eventManager->RegisterHandler(Event::KeyReleased, bind(&AnimationEditor::OnKeyReleased, this, _1));

  _windowManager.AddWindow(new AnimationWindow("ANIMATION", Vector2f(20, 20), Vector2f(200,200), this));
  _windowManager.AddWindow(new CanvasWindow("CANVAS", Vector2f(250,150), Vector2f(400,400), this));

  _windowManager.AddWindow(new ToolkitWindow("TOOLKIT", Vector2f(250,20), Vector2f(400,100)));
  _windowManager.AddWindow(new ColorPickerWindow("COLORPICKER", Vector2f(750,20), Vector2f(350,200), this));
}

//-----------------------------------------------------------------------------
bool AnimationEditor::Init()
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
Vector2f AnimationEditor::GetFrameSize() const
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
bool AnimationEditor::LoadImage(const char* filename)
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
bool AnimationEditor::OnKeyReleased(const Event& event)
{
  // Where applicable, the shortcuts are copied from photoshop

  int _oldZoom = _curZoom;

  Keyboard::Key code = event.key.code;

  if (code == Keyboard::I)
  {
    _eyeDropper = true;
  }
  else if (code == Keyboard::X)
  {
    swap(_primaryColor, _secondaryColor);
  }
  else if (code >= Keyboard::Num0 && code <= Keyboard::Num9)
  {
    // transfer swatch <-> primary
    int idx = (int)code - Keyboard::Num0;
    if (event.key.control)
      _swatch[idx] = _primaryColor;
    else
      _primaryColor = _swatch[idx];
  }
  else if (code == Keyboard::Key::Up)
  {
    _curAnimation++;
  }
  else if (code == Keyboard::Key::Down)
  {
    _curAnimation = max(0, _curAnimation - 1);
  }
  else if (code == Keyboard::Key::Add)
  {
    _curZoom = min(10, _curZoom + 1);
  }
  else if (code == Keyboard::Key::Subtract)
  {
    _curZoom = max(1, _curZoom - 1);
  }
  else if (code == Keyboard::Key::Space)
  {
    _playOnce = false;
  }
  else if (code == Keyboard::Key::Return)
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
void AnimationEditor::Update()
{
  _windowManager.Update();
}
