#pragma once
#include "animation.hpp"

namespace rogue
{
  class AnimationManager
  {
  public:
    ~AnimationManager();

    void CheckForReload();
    bool LoadAnimations(const char* filename);
    Animation* FindAnimation(Animation::Id id);

    void GetAnimations(vector<Animation*>* animations);

  private:
    map<Animation::Id, Animation*> _animationMap;
    map<string, time_t> _lastModification;
  };
}