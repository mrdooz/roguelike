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
    void Update();
  private:

    ptime _lastFrame;
    HotloadSprite _animationSprite;
    vector<HotloadSprite> _animationFrames;

    RenderWindow *_window;
    Font* _font;

    int _curAnimation;
    int _curZoom;
    bool _playOnce;
  };
}
