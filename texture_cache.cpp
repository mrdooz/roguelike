#include "texture_cache.hpp"
#include "utils.hpp"

using namespace rogue;

//-----------------------------------------------------------------------------
TextureCache::~TextureCache()
{
  assoc_delete(&_textureCache);
  assoc_delete(&_fontCache);
}

//-----------------------------------------------------------------------------
Texture* TextureCache::LoadTexture(const string& filename)
{
  return LoadFromCache(filename, _textureCache);
}

//-----------------------------------------------------------------------------
Font* TextureCache::LoadFont(const string& filename)
{
  return LoadFromCache(filename, _fontCache);
}

//-----------------------------------------------------------------------------
template <typename T>
T* TextureCache::LoadFromCache(const string& filename, unordered_map<string, T*>& cache)
{
  auto it = cache.find(filename);
  if (it != cache.end())
  {
    return it->second;
  }

  T* resource = new T();
  if (!resource->loadFromFile(filename))
  {
    delete resource;
    return nullptr;
  }

  cache[filename] = resource;
  return resource;
}

