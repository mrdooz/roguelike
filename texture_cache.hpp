#pragma once
#include "texture_handle.hpp"

namespace rogue
{
  class TextureCache
  {
  public:
    struct TextureEntry
    {
      TextureEntry() : _texture(nullptr) {}
      Texture* _texture;
      time_t _lastModification;
      string _filename;
    };

    TextureCache();
    ~TextureCache();

    void CheckForReload();
    TextureHandle LoadTextureByHandle(const string& filename);
    Texture* TextureByHandle(const TextureHandle& handle);
    const TextureEntry* GetTextureEntry(const TextureHandle& handle) const;

  private:

    ptime _lastUpdate;
    TextureEntry _elems[4096];
    u16 _nextIdx;
    unordered_map<string, TextureHandle> _filenameToHandle;
  };
}
