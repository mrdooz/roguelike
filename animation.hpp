#pragma once

#include "texture_handle.hpp"
#include "hotload_sprite.hpp"

namespace rogue
{
  struct Frame
  {
    int _weight;
    IntRect _textureRect;
  };

  struct Animation
  {
    enum class Id
    {
      None,
      Blood,
      ArcaneBlast,
      LightningBolt,
    };

    Animation(Id id, const TextureHandle& texture, time_duration duration);
    Id _id;
    TextureHandle _texture;
    time_duration _duration;
    vector<Frame> _frames;
    // for each index [0,weightSum), which frame should be drawn
    vector<u32> _frameIndex;
    u32 _weightSum;
    bool _looping;
  };

  struct AnimationInstance
  {
    AnimationInstance();
    HotloadSprite _sprite;
    Animation* _animation;
    ptime _startTime;
    ptime _endTime;
    time_duration _duration;
    Pos _startPos;
    Pos _endPos;
  };

}