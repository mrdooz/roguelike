#pragma once

namespace rogue
{
  struct GameState;

  class DebugRenderer
  {
  public:
    DebugRenderer(RenderWindow *window);

    bool Init();
    void Resize();
    void Update(const GameState& state, const Event* event);
  private:

    RenderWindow *_window;
    Font* _font;
  };

}