#pragma once
#include "hotload_sprite.hpp"
#include "virtual_window_manager.hpp"

namespace rogue
{
  class GameState;

    class DebugRenderer
  {
  public:
    DebugRenderer(RenderWindow *window, WindowEventManager* eventManager);

    bool Init();
    bool OnResize(const Event& event);
    bool OnKeyReleased(const Event& event);
    bool OnMouseButtonDown(const Event& event);
    bool OnMouseMove(const Event& event);
    void Update();
  private:

    void DrawAnimationWidget();
    void DrawCanvasWidget();
    void DrawColorPickerWidget();
    void DrawFramesWidget();

    void DrawEditor();
    void DrawGrid();

    bool LoadImage(const char* filename);
    Vector2i ImageOffset();


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

    ptime _lastFrame;
    HotloadSprite _animationSprite;
    vector<HotloadSprite> _animationFrames;

    RenderWindow *_window;
    VirtualWindowManager _windowManager;
    Font* _font;

    vector<u8> _doubleBuffer;
    Texture _editorTexture;
    Image _editorImage;
    Sprite _editorSprite;

    Vector2u _gridSize;
    Vector2u _imageSize;
    int _curAnimation;
    int _curZoom;
    bool _playOnce;

    bool _showGrid;
  };
}
