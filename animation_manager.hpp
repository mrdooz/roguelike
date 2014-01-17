#pragma once
#include "animation.hpp"

namespace rogue
{
  class TextureCache;

  class AnimationManager
  {
  public:

    typedef function<void()> fnAnimationsUpdated;

    AnimationManager();
    ~AnimationManager();

    void CheckForReload();
    bool LoadAnimations(const char* filename);
    Animation* FindAnimation(Animation::Id id);

    void GetAnimations(vector<Animation*>* animations);
    bool GetFrame(u32 animationIdx, u32 frameIdx, Frame* frame);

    u32 AddUpdateListener(const fnAnimationsUpdated& listener);
    void RemoveListener(u32 id);

  private:
    map<Animation::Id, Animation*> _animationMap;
    map<string, time_t> _lastModification;

    u32 _nextId;
    map<u32, fnAnimationsUpdated> _listeners;
  };
}