#pragma once
#include "hotload_sprite.hpp"

namespace rogue
{
  class GameState;

  class DebugRenderer
  {
  public:
    DebugRenderer(RenderWindow *window);

    bool Init();
    bool OnResize(const Event& event);
    bool OnKeyReleased(const Event& event);
    bool OnMouseButtonDown(const Event& event);
    bool OnMouseMove(const Event& event);
    void Update();
  private:

    void DrawAnimation();
    void DrawEditor();
    void DrawGrid();

    bool LoadImage(const char* filename);
    Vector2i ImageOffset();

    ptime _lastFrame;
    HotloadSprite _animationSprite;
    vector<HotloadSprite> _animationFrames;

    RenderWindow *_window;
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
