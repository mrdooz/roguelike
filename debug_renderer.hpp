#pragma once
#include "hotload_sprite.hpp"
#include "virtual_window_manager.hpp"
#include "virtual_window.hpp"

namespace rogue
{
  class GameState;
  class DebugRenderer;

  //-----------------------------------------------------------------------------
  class AnimationWindow : public VirtualWindow
  {
    public:
    AnimationWindow(const string& title, const Vector2f& pos, const Vector2f& size, DebugRenderer* renderer);
    virtual bool Init();
    virtual void Draw();

    bool OnKeyReleased(const Event& event);

    HotloadSprite _animationSprite;
    vector<HotloadSprite> _animationFrames;

    DebugRenderer* _renderer;
    ptime _lastFrame;

    float _curZoom;
  };

  //-----------------------------------------------------------------------------
  class CanvasWindow : public VirtualWindow
  {
  public:
    CanvasWindow(const string& title, const Vector2f& pos, const Vector2f& size, DebugRenderer* renderer);
    bool Init();
    bool LoadImage(const char* filename);
    Vector2f ImageOffset();
    bool OnMouseMove(const Event& event);
    bool OnKeyReleased(const Event& event);
    void Draw();

    void UpdateDoubleBuffer();

    DebugRenderer* _renderer;

    Image _editorImage;

    Texture _frameTexture;
    Sprite _frameSprite;

    vector<u8> _imageDoubleBuffer;
    vector<u8> _frameDoubleBuffer;
//    vector<u8> _doubleBuffer;
    Texture _editorTexture;
    Sprite _editorSprite;
    
    Vector2u _gridSize;
    Vector2u _imageSize;
  };

  //-----------------------------------------------------------------------------
  class ToolkitWindow : public VirtualWindow
  {
  public:
    ToolkitWindow(const string& title, const Vector2f& pos, const Vector2f& size);
    virtual void Draw();
  };

  //-----------------------------------------------------------------------------
  class ColorPickerWindow : public VirtualWindow
  {
  public:
    ColorPickerWindow(const string& title, const Vector2f& pos, const Vector2f& size);
    virtual void Draw();
  };

  //-----------------------------------------------------------------------------
  class FramesWindow : public VirtualWindow
  {
  public:
    FramesWindow(const string& title, const Vector2f& pos, const Vector2f& size);
    virtual void Draw();
  };

  //-----------------------------------------------------------------------------
  class DebugRenderer
  {
    friend class AnimationWindow;
    friend class CanvasWindow;
    friend class ToolkitWindow;
    friend class ColorPickerWindow;
    friend class FramesWindow;

  public:
    DebugRenderer(RenderWindow *window, WindowEventManager* eventManager);

    bool Init();
    bool OnResize(const Event& event);
    bool OnKeyReleased(const Event& event);
    bool OnMouseButtonDown(const Event& event);
    bool OnMouseMove(const Event& event);
    void Update();

    Vector2f GetFrameSize() const;

  private:

    void DrawEditor();
    void DrawGrid();

    bool LoadImage(const char* filename);
    Vector2i ImageOffset();

    Image _editorImage;
/*
    RenderTexture _rtAnimation;
    RenderTexture _rtMenu;
    RenderTexture _rtCanvas;
    RenderTexture _rtColorPicker;
    RenderTexture _rtFrames;

    Sprite _sprAnimation;
    Sprite _sprMenu;
    Sprite _sprCanvas;
    Sprite _sprColorPicker;
    Sprite _sprFrames;
*/

    RenderWindow *_window;
    VirtualWindowManager _windowManager;
    Font* _font;

    int _curAnimation;
    int _curFrame;
    int _curZoom;
    bool _playOnce;

    bool _showGrid;
  };
}
