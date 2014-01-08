#pragma once

namespace rogue
{

  class VirtualWindowManager;

  class VirtualWindow
  {
    friend class VirtualWindowManager;
  public:
    virtual void Draw() = 0;
  protected:
    VirtualWindow(
        const string& title,
        const Vector2u& pos,
        const Vector2u& size);

    VirtualWindowManager* _windowManager;

    Sprite _sprite;
    RenderTexture _texture;

    string _title;
    Vector2u _pos;
    Vector2u _size;
    size_t _borderWidth;
    bool _focus;
  };
}