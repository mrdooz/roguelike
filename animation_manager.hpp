#pragma once
#include "animation.hpp"

namespace rogue
{
  class AnimationManager
  {
  public:
    ~AnimationManager();
    bool LoadAnimations(const char* filename);
    Animation* FindAnimation(Animation::Id id);

  private:
    map<Animation::Id, Animation*> _animationMap;

  };
}