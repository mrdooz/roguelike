#pragma once

namespace rogue
{
  class TextureHandle
  {
    friend class TextureCache;
  public:
    TextureHandle() : _idx(~0), _generation(~0), _type(Type::Unknown) {}
    bool IsValid() const { return _type != Type::Unknown; }

    operator bool() { return IsValid(); }
  private:
    enum class Type : u8
    {
      Unknown,
      Texture,
      Font,
    };
    TextureHandle(Type type, u16 idx) : _idx(idx), _generation(0), _type(type) {}
    u16 _idx;
    u8 _generation;
    Type _type;
  };

}