#pragma once

#include "texture_handle.hpp"
#include "hotload_sprite.hpp"

namespace rogue
{
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
    vector<IntRect> _textureRects;
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