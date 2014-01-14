#include "animation_manager.hpp"
#include "utils.hpp"
#include "game.hpp"
#include "texture_cache.hpp"
#include "protocol/animation_config.pb.h"
#include "error.hpp"

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
  if (!LoadFile(filename, &str))
  {
    LOG_WARN("Error loading animation pb" << LogKeyValue("filename", filename));
    return false;
  }

  if (!TextFormat::ParseFromString(str, &animations))
    return false;

  for (int i = 0; i < animations.animation_size(); ++i)
  {
    auto& cur = animations.animation(i);
    TextureHandle texture = TEXTURE_CACHE->LoadTextureByHandle(cur.texture());
    if (!texture)
    {
      LOG_WARN("Unable to find texture" << LogKeyValue("filename", cur.texture()));
      return false;
    }
    Animation* anim = new Animation((Animation::Id)cur.id(), texture, milliseconds(cur.duration_ms()));
    anim->_looping = cur.looping();

    // Load the animation frames
    for (int j = 0; j < cur.frame_size(); ++j)
    {
      auto& curFrame = cur.frame(j);
      Frame frame;
      frame._weight = curFrame.weight();
      auto& textureRect = cur.frame(j).texture_rect();
      frame._textureRect = IntRect(textureRect.x(), textureRect.y(), textureRect.w(), textureRect.h());
      anim->_frames.push_back(frame);

      // update the weight table with the current frame
      anim->_weightSum += frame._weight;
      for (int k = 0; k < frame._weight; ++k)
        anim->_frameIndex.push_back(j);
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

//-----------------------------------------------------------------------------
bool AnimationManager::GetFrame(u32 animationIdx, u32 frameIdx, Frame* frame)
{
  vector<Animation*> animations;
  GetAnimations(&animations);
  if (animationIdx >= animations.size())
    return false;

  Animation* animation = animations[animationIdx];
  if (frameIdx > animation->_frames.size())
    return false;

  *frame = animation->_frames[frameIdx];
  return true;
}
