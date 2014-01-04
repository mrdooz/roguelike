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
    _animationMap[anim->_id] = anim;
  }

  return true;
}
