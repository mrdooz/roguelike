#include "animation_manager.hpp"
#include "utils.hpp"
#include "game.hpp"
#include "texture_cache.hpp"
#include "protocol/animation_config.pb.h"

using namespace rogue;

//-----------------------------------------------------------------------------
AnimationManager::~AnimationManager()
{
  for (const auto& a : _animationMap)
    delete a.second;
  _animationMap.clear();
}

//-----------------------------------------------------------------------------
void AnimationManager::CheckForReload()
{
  for (auto it = _lastModification.begin(); it != _lastModification.end(); ++it)
  {
    const char* filename = it->first.c_str();
    struct stat s;
    stat(filename, &s);
    if (s.st_mtime > it->second)
    {
      // File on disk is newer
      LoadAnimations(filename);
      it->second = s.st_mtime;
    }
  }
}

//-----------------------------------------------------------------------------
Animation* AnimationManager::FindAnimation(Animation::Id id)
{
  auto it = _animationMap.find(id);
  return it == _animationMap.end() ? nullptr : it->second;
}

//-----------------------------------------------------------------------------
bool AnimationManager::LoadAnimations(const char* filename)
{
  rogue::animation_config::Animations animations;

  string str;
  if (!LoadFile("config/animation_config.pb", &str))
    return false;

  if (!TextFormat::ParseFromString(str, &animations))
    return false;

  for (int i = 0; i < animations.animation_size(); ++i)
  {
    auto& cur = animations.animation(i);
    TextureHandle texture = TEXTURE_CACHE->LoadTextureByHandle(cur.texture());
    if (!texture)
    {
      //LOG_WARN("unable to find texture")..
      return false;
    }
    Animation* anim = new Animation((Animation::Id)cur.id(), texture, milliseconds(cur.duration_ms()));
    anim->_looping = cur.looping();

    // Load the animation frames
    for (int j = 0; j < cur.frame_size(); ++j)
    {
      auto& curFrame = cur.frame(j);
      anim->_textureRects.push_back(IntRect(curFrame.x(), curFrame.y(), curFrame.w(), curFrame.h()));
    }

    Animation*& prev = _animationMap[anim->_id];
    delete exch_null(prev);
    prev = anim;

    // Add file to last modification map
    auto it = _lastModification.find(filename);
    if (it == _lastModification.end())
    {
      struct stat s;
      stat(filename, &s);
      _lastModification[filename] = s.st_mtime;
    }
  }
  return true;
}

//-----------------------------------------------------------------------------
void AnimationManager::GetAnimations(vector<Animation*>* animations)
{
  animations->clear();
  for (auto& a: _animationMap)
  {
    animations->push_back(a.second);
  }
}
