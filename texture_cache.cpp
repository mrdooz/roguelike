#include "texture_cache.hpp"
#include "utils.hpp"
#include "error.hpp"

using namespace rogue;

TextureCache* TextureCache::_instance;

//-----------------------------------------------------------------------------
bool TextureCache::Close()
{
  assert(_instance);
  delete exch_null(_instance);
  return true;
}

//-----------------------------------------------------------------------------
bool TextureCache::Create()
{
  assert(!_instance);
  _instance = new TextureCache();
  return true;
}

//-----------------------------------------------------------------------------
TextureCache* TextureCache::Instance()
{
  assert(_instance);
  return _instance;
}

//-----------------------------------------------------------------------------
TextureCache::TextureCache()
  : _lastUpdate(microsec_clock::local_time())
  , _nextIdx(0)
{
}

//-----------------------------------------------------------------------------
TextureCache::~TextureCache()
{
  for (auto& e : _elems)
  {
    delete exch_null(e._texture);
  }
}

//-----------------------------------------------------------------------------
TextureHandle TextureCache::LoadTextureByHandle(const string& filename)
{
  auto it = _filenameToHandle.find(filename);
  if (it != _filenameToHandle.end())
    return it->second;

  auto texture = new Texture();
  TextureHandle handle;
  if (!texture->loadFromFile(filename))
  {
    delete texture;
    return handle;
  }

  // Assign the next free index
  if (_nextIdx == ELEMS_IN_ARRAY(_elems) - 1)
  {
    LOG_WARN("Texture cache too small!");
    return handle;
  }

  // Save the last modification time
  struct stat s;
  stat(filename.c_str(), &s);
  TextureEntry& entry = _elems[_nextIdx];
  entry._filename = filename;
  entry._texture = texture;
  entry._lastModification = s.st_mtime;

  return TextureHandle(TextureHandle::Type::Texture, _nextIdx++);
}

//-----------------------------------------------------------------------------
Texture* TextureCache::TextureByHandle(const TextureHandle& handle)
{
  return _elems[handle._idx]._texture;
}

//-----------------------------------------------------------------------------
const TextureCache::TextureEntry* TextureCache::GetTextureEntry(
    const TextureHandle& handle) const
{
  if (handle._idx >= ELEMS_IN_ARRAY(_elems))
    return nullptr;

  return &_elems[handle._idx];
}

//-----------------------------------------------------------------------------
void TextureCache::CheckForReload()
{
  // only update once per second
  ptime now = microsec_clock::local_time();
  if (now - _lastUpdate < seconds(1))
    return;

  _lastUpdate = now;

  for (int i = 0; i < _nextIdx; ++i)
  {
    // Check the last modification time for all the textures, and reload any that
    // have been updated
    auto& elem = _elems[i];
    struct stat s;
    stat(elem._filename.c_str(), &s);
    if (s.st_mtime > elem._lastModification)
    {
      Texture* t = new Texture();
      if (t->loadFromFile(elem._filename))
      {
        delete exch_null(elem._texture);
        elem._texture = t;
      }
      else
      {
        LOG_INFO("Error loading texture.."
              << LogKeyValue("filename", elem._filename));
      }
    }
  }
}

