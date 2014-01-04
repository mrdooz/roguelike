#pragma once
#include "texture_handle.hpp"

namespace rogue
{
  class TextureCache
  {
  public:
    TextureCache();
    ~TextureCache();

    void CheckForUpdates();
    TextureHandle LoadTextureByHandle(const string& filename);
    Texture* TextureByHandle(const TextureHandle& handle);

  private:
    struct TextureEntry
    {
      TextureEntry() : _texture(nullptr) {}
      Texture* _texture;
      time_t _last_modification;
      string _filename;
    };

    ptime _lastUpdate;
    TextureEntry _elems[4096];
    u16 _nextIdx;
    unordered_map<string, TextureHandle> _filenameToHandle;
  };
}
