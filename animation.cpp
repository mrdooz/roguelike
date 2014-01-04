#include "animation.hpp"

using namespace rogue;

//-----------------------------------------------------------------------------
Animation::Animation(Id id, const TextureHandle& texture, time_duration duration)
  : _id(id)
  , _texture(texture)
  , _duration(duration)
  , _weightSum(0)
  , _looping(false)
{
}

//-----------------------------------------------------------------------------
AnimationInstance::AnimationInstance()
  : _animation(nullptr)
{
  _sprite.setScale(3, 3);
}
