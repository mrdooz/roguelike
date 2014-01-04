#pragma once

namespace rogue
{
  class GameState;

  class DebugRenderer
  {
  public:
    DebugRenderer(RenderWindow *window);

    bool Init();
    bool OnResize(const Event& event);
    void Update();
  private:

    RenderWindow *_window;
    Font* _font;
  };

}