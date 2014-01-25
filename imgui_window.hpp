#pragma once
#include "virtual_window.hpp"

namespace rogue
{
  struct UIState
  {
    UIState();
    int mouseX;
    int mouseY;
    int mouseDown;
    // item below the mouse cursor
    int hotItem;
    int activeItem;

    int keyboardItem;
    Keyboard::Key key;
    int keyMod;
    int lastItem;
    int keyChar;
  };

  class ImGuiWindow : public VirtualWindow
  {
  public:
    ImGuiWindow(const string& title, const Vector2f& pos, const Vector2f& size);
    bool Button(int id, int x, int y, int w, int h);
    bool TextField(int id, int x, int y, int maxLen, int h, char* buffer);
    void Label(int x, int y, int h, const char* str);

  private:
    virtual void Draw();
    virtual bool Init();

    bool RegionHit(int x, int y, int w, int h);

    void ImGuiPrepare();
    void ImGuiFinish();
    void DrawRect(int x, int y, int w, int h, const Color& color = Color::White);

    bool OnMouseMove(const Event& event);
    bool OnMouseButtonReleased(const Event& event);
    bool OnMouseButtonPressed(const Event& event);
    bool OnKeyPressed(const Event& event);
    bool OnKeyReleased(const Event& event);
    bool OnTextEntered(const Event& event);

    UIState _uistate;
    Font _font;
  };
}
