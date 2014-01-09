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
        const Vector2f& pos,
        const Vector2f& size);

    void SetPosition(const Vector2f& pos);

    VirtualWindowManager* _windowManager;

    Sprite _sprite;
    RenderTexture _texture;

    string _title;
    Vector2f _pos;
    Vector2f _size;
    int _depth;
    u32 _borderWidth;
    u32 _titleBarHeight;
    bool _focus;
    bool _moving;
  };
}