#pragma once
#include "hotload_sprite.hpp"
#include "virtual_window_manager.hpp"
#include "virtual_window.hpp"
#include "sfml_helpers.hpp"
#include "animation.hpp"

namespace rogue
{
  class GameState;
  class AnimationEditor;
  class AnimationManager;
  class TextureCache;

  //-----------------------------------------------------------------------------
  class HistoryWindow : public VirtualWindow
  {
  public:
    HistoryWindow(const string& title, const Vector2f& pos, const Vector2f& size, AnimationEditor* editor);
    virtual void Draw();
    void AddHistory(const string& str);

    vector<string> _history;
    AnimationEditor* _editor;
  };

  //-----------------------------------------------------------------------------
  class AnimationWindow : public VirtualWindow
  {
    public:
    AnimationWindow(const string& title, const Vector2f& pos, const Vector2f& size, AnimationEditor* editor);
    virtual bool Init();
    virtual void Draw();

    bool OnKeyReleased(const Event& event);

    Sprite _animationSprite;
    vector<Sprite> _animationFrames;

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
    bool OnMouseMoved(const Event& event);
    bool OnMouseButtonPressed(const Event& event);
    bool OnKeyReleased(const Event& event);
    bool OnResized(const Event& event);
    void Draw();

    void UpdateFrameTexture();
    void UpdateDoubleBuffer();
    void UpdateFrameBuffer(int x, int y, sf::Mouse::Button btn);

    void TextureReloaded();

    void CopyToClipboard();
    void PasteFromClipboard();
    void Undo();
    void Redo();

    void SaveToUndoBuffer(int x, int y, u32 col);
    void ResetUndoBuffer();

    void LightningBolt();

    AnimationEditor* _editor;

    Texture _frameTexture;
    Sprite _frameSprite;

    vector<u8> _clipboard;

    vector<u8> _imageDoubleBuffer;
    vector<u8> _frameDoubleBuffer;
    
    Vector2u _gridSize;
    int _scaleX;
    int _scaleY;
    Vector2i _frameSize;
    bool _showGrid;
    bool _showPrevFrame;

    // Points to next slot in the undo buffer
    int _undoIndex;
    struct UndoState
    {
      int x, y;
      u32 col;
    };

    vector<UndoState> _undoBuffer;
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
    friend class HistoryWindow;
  public:
    AnimationEditor(
        RenderWindow *window,
        WindowEventManager* eventManager,
        AnimationManager* animationManager);

    bool Init();
    bool OnKeyReleased(const Event& event);
    void Update();

  private:
    Vector2f GetFrameSize() const;

    void DrawEditor();
    void DrawGrid();

    void AnimationsReloaded();
    void SaveAnimation();

    Image _editorImage;

    RenderWindow *_window;
    VirtualWindowManager _windowManager;
    Font* _font;

    // The animation editor keeps its own local copy of the texture
    // that's currently being edited
    Texture* _curTexture;
    Animation* _curAnimation;
    Frame _curFrame;
    int _curAnimationIdx;
    int _curFrameIdx;
    bool _playOnce;

    HsvColor _primaryColor;
    HsvColor _secondaryColor;
    vector<HsvColor> _swatch;

    bool _eyeDropper;
    bool _selectionMode;

    AnimationWindow* _animationWindow;
    CanvasWindow* _canvasWindow;
    ToolkitWindow* _toolkitWindow;
    ColorPickerWindow* _colorPickerWindow;
    AnimationManager* _animationManager;
    HistoryWindow* _historyWindow;
  };
}
