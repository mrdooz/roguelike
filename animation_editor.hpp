#pragma once
#include "hotload_sprite.hpp"
#include "virtual_window_manager.hpp"
#include "virtual_window.hpp"
#include "sfml_helpers.hpp"

namespace rogue
{
  class GameState;
  class AnimationEditor;

  //-----------------------------------------------------------------------------
  class AnimationWindow : public VirtualWindow
  {
    public:
    AnimationWindow(const string& title, const Vector2f& pos, const Vector2f& size, AnimationEditor* editor);
    virtual bool Init();
    virtual void Draw();

    bool OnKeyReleased(const Event& event);

    HotloadSprite _animationSprite;
    vector<HotloadSprite> _animationFrames;

    AnimationEditor* _editor;
    ptime _lastFrame;

    float _curZoom;
  };

  //-----------------------------------------------------------------------------
  class CanvasWindow : public VirtualWindow
  {
  public:
    CanvasWindow(const string& title, const Vector2f& pos, const Vector2f& size, AnimationEditor* editor);
    bool Init();
    bool LoadImage(const char* filename);
    bool OnMouseMove(const Event& event);
    bool OnMouseButtonPressed(const Event& event);
    bool OnKeyReleased(const Event& event);
    void Draw();

    void UpdateDoubleBuffer();
    void UpdateFrameBuffer(int x, int y, sf::Mouse::Button btn);

    AnimationEditor* _editor;

    Image _editorImage;

    Texture _frameTexture;
    Sprite _frameSprite;

    vector<u8> _imageDoubleBuffer;
    vector<u8> _frameDoubleBuffer;
    Texture _editorTexture;
    Sprite _editorSprite;
    
    Vector2u _gridSize;
    Vector2u _imageSize;
    int _scale;
    Vector2i _frameSize;
    bool _showGrid;
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
    ColorPickerWindow(const string& title, const Vector2f& pos, const Vector2f& size, AnimationEditor* editor);
    virtual void Draw();
    virtual bool Init();

    void UpdateHsv(int x, int y);
    bool OnMousePressed(const Event& event);
    bool OnMouseMoved(const Event& event);
    bool OnResized(const Event& event);
    bool OnKeyReleased(const Event& event);

    void UpdatePicker();

    void DrawSliders();
    void DrawSwatches();

    AnimationEditor* _editor;

    int _valueWidth;
    int _swatchSize;
    
    Texture _pickerHTexture;
    Texture _pickerSTexture;
    Texture _pickerVTexture;

    Sprite _pickerHSprite;
    Sprite _pickerSSprite;
    Sprite _pickerVSprite;
  };

  //-----------------------------------------------------------------------------
  class AnimationEditor
  {
    friend class AnimationWindow;
    friend class CanvasWindow;
    friend class ToolkitWindow;
    friend class ColorPickerWindow;
    friend class FramesWindow;

  public:
    AnimationEditor(RenderWindow *window, WindowEventManager* eventManager);

    bool Init();
    bool OnKeyReleased(const Event& event);
    void Update();

    Vector2f GetFrameSize() const;

  private:

    void DrawEditor();
    void DrawGrid();

    bool LoadImage(const char* filename);

    Image _editorImage;

    RenderWindow *_window;
    VirtualWindowManager _windowManager;
    Font* _font;

    int _curAnimation;
    int _curFrame;
    int _curZoom;
    bool _playOnce;

    HsvColor _primaryColor;
    HsvColor _secondaryColor;
    vector<HsvColor> _swatch;

    bool _eyeDropper;
  };
}
