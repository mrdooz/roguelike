#pragma once

namespace rogue
{

  // supersimple texture cache, just so i can map from string -> texture for config
  class TextureCache
  {
  public:
    ~TextureCache();

    Texture* LoadTexture(const string& filename);
    Font* LoadFont(const string& filename);

  private:
    template <typename T>
    T* LoadFromCache(const string& filename, unordered_map<string, T*>& cache);

    unordered_map<string, Texture*> _textureCache;
    unordered_map<string, Font*> _fontCache;
  };

}