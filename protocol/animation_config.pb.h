// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: animation_config.proto

#ifndef PROTOBUF_animation_5fconfig_2eproto__INCLUDED
#define PROTOBUF_animation_5fconfig_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2005000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2005000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/generated_enum_reflection.h>
#include <google/protobuf/unknown_field_set.h>
#include "sprite.pb.h"
// @@protoc_insertion_point(includes)

namespace rogue {
namespace animation_config {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_animation_5fconfig_2eproto();
void protobuf_AssignDesc_animation_5fconfig_2eproto();
void protobuf_ShutdownFile_animation_5fconfig_2eproto();

class Animation;
class Animations;

enum AnimationId {
  None = 0,
  Blood = 1,
  ArcaneBlast = 2,
  LightningBolt = 3
};
bool AnimationId_IsValid(int value);
const AnimationId AnimationId_MIN = None;
const AnimationId AnimationId_MAX = LightningBolt;
const int AnimationId_ARRAYSIZE = AnimationId_MAX + 1;

const ::google::protobuf::EnumDescriptor* AnimationId_descriptor();
inline const ::std::string& AnimationId_Name(AnimationId value) {
  return ::google::protobuf::internal::NameOfEnum(
    AnimationId_descriptor(), value);
}
inline bool AnimationId_Parse(
    const ::std::string& name, AnimationId* value) {
  return ::google::protobuf::internal::ParseNamedEnum<AnimationId>(
    AnimationId_descriptor(), name, value);
}
// ===================================================================

class Animation : public ::google::protobuf::Message {
 public:
  Animation();
  virtual ~Animation();

  Animation(const Animation& from);

  inline Animation& operator=(const Animation& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const Animation& default_instance();

  void Swap(Animation* other);

  // implements Message ----------------------------------------------

  Animation* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Animation& from);
  void MergeFrom(const Animation& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional .rogue.animation_config.AnimationId id = 1;
  inline bool has_id() const;
  inline void clear_id();
  static const int kIdFieldNumber = 1;
  inline ::rogue::animation_config::AnimationId id() const;
  inline void set_id(::rogue::animation_config::AnimationId value);

  // optional string texture = 2;
  inline bool has_texture() const;
  inline void clear_texture();
  static const int kTextureFieldNumber = 2;
  inline const ::std::string& texture() const;
  inline void set_texture(const ::std::string& value);
  inline void set_texture(const char* value);
  inline void set_texture(const char* value, size_t size);
  inline ::std::string* mutable_texture();
  inline ::std::string* release_texture();
  inline void set_allocated_texture(::std::string* texture);

  // optional int32 duration_ms = 3 [default = 1000];
  inline bool has_duration_ms() const;
  inline void clear_duration_ms();
  static const int kDurationMsFieldNumber = 3;
  inline ::google::protobuf::int32 duration_ms() const;
  inline void set_duration_ms(::google::protobuf::int32 value);

  // repeated .rogue.sprite.TextureRect frame = 4;
  inline int frame_size() const;
  inline void clear_frame();
  static const int kFrameFieldNumber = 4;
  inline const ::rogue::sprite::TextureRect& frame(int index) const;
  inline ::rogue::sprite::TextureRect* mutable_frame(int index);
  inline ::rogue::sprite::TextureRect* add_frame();
  inline const ::google::protobuf::RepeatedPtrField< ::rogue::sprite::TextureRect >&
      frame() const;
  inline ::google::protobuf::RepeatedPtrField< ::rogue::sprite::TextureRect >*
      mutable_frame();

  // optional bool looping = 5 [default = false];
  inline bool has_looping() const;
  inline void clear_looping();
  static const int kLoopingFieldNumber = 5;
  inline bool looping() const;
  inline void set_looping(bool value);

  // @@protoc_insertion_point(class_scope:rogue.animation_config.Animation)
 private:
  inline void set_has_id();
  inline void clear_has_id();
  inline void set_has_texture();
  inline void clear_has_texture();
  inline void set_has_duration_ms();
  inline void clear_has_duration_ms();
  inline void set_has_looping();
  inline void clear_has_looping();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::std::string* texture_;
  int id_;
  ::google::protobuf::int32 duration_ms_;
  ::google::protobuf::RepeatedPtrField< ::rogue::sprite::TextureRect > frame_;
  bool looping_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(5 + 31) / 32];

  friend void  protobuf_AddDesc_animation_5fconfig_2eproto();
  friend void protobuf_AssignDesc_animation_5fconfig_2eproto();
  friend void protobuf_ShutdownFile_animation_5fconfig_2eproto();

  void InitAsDefaultInstance();
  static Animation* default_instance_;
};
// -------------------------------------------------------------------

class Animations : public ::google::protobuf::Message {
 public:
  Animations();
  virtual ~Animations();

  Animations(const Animations& from);

  inline Animations& operator=(const Animations& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const Animations& default_instance();

  void Swap(Animations* other);

  // implements Message ----------------------------------------------

  Animations* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Animations& from);
  void MergeFrom(const Animations& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // repeated .rogue.animation_config.Animation animation = 1;
  inline int animation_size() const;
  inline void clear_animation();
  static const int kAnimationFieldNumber = 1;
  inline const ::rogue::animation_config::Animation& animation(int index) const;
  inline ::rogue::animation_config::Animation* mutable_animation(int index);
  inline ::rogue::animation_config::Animation* add_animation();
  inline const ::google::protobuf::RepeatedPtrField< ::rogue::animation_config::Animation >&
      animation() const;
  inline ::google::protobuf::RepeatedPtrField< ::rogue::animation_config::Animation >*
      mutable_animation();

  // @@protoc_insertion_point(class_scope:rogue.animation_config.Animations)
 private:

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::RepeatedPtrField< ::rogue::animation_config::Animation > animation_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];

  friend void  protobuf_AddDesc_animation_5fconfig_2eproto();
  friend void protobuf_AssignDesc_animation_5fconfig_2eproto();
  friend void protobuf_ShutdownFile_animation_5fconfig_2eproto();

  void InitAsDefaultInstance();
  static Animations* default_instance_;
};
// ===================================================================


// ===================================================================

// Animation

// optional .rogue.animation_config.AnimationId id = 1;
inline bool Animation::has_id() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void Animation::set_has_id() {
  _has_bits_[0] |= 0x00000001u;
}
inline void Animation::clear_has_id() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void Animation::clear_id() {
  id_ = 0;
  clear_has_id();
}
inline ::rogue::animation_config::AnimationId Animation::id() const {
  return static_cast< ::rogue::animation_config::AnimationId >(id_);
}
inline void Animation::set_id(::rogue::animation_config::AnimationId value) {
  assert(::rogue::animation_config::AnimationId_IsValid(value));
  set_has_id();
  id_ = value;
}

// optional string texture = 2;
inline bool Animation::has_texture() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void Animation::set_has_texture() {
  _has_bits_[0] |= 0x00000002u;
}
inline void Animation::clear_has_texture() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void Animation::clear_texture() {
  if (texture_ != &::google::protobuf::internal::kEmptyString) {
    texture_->clear();
  }
  clear_has_texture();
}
inline const ::std::string& Animation::texture() const {
  return *texture_;
}
inline void Animation::set_texture(const ::std::string& value) {
  set_has_texture();
  if (texture_ == &::google::protobuf::internal::kEmptyString) {
    texture_ = new ::std::string;
  }
  texture_->assign(value);
}
inline void Animation::set_texture(const char* value) {
  set_has_texture();
  if (texture_ == &::google::protobuf::internal::kEmptyString) {
    texture_ = new ::std::string;
  }
  texture_->assign(value);
}
inline void Animation::set_texture(const char* value, size_t size) {
  set_has_texture();
  if (texture_ == &::google::protobuf::internal::kEmptyString) {
    texture_ = new ::std::string;
  }
  texture_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* Animation::mutable_texture() {
  set_has_texture();
  if (texture_ == &::google::protobuf::internal::kEmptyString) {
    texture_ = new ::std::string;
  }
  return texture_;
}
inline ::std::string* Animation::release_texture() {
  clear_has_texture();
  if (texture_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = texture_;
    texture_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void Animation::set_allocated_texture(::std::string* texture) {
  if (texture_ != &::google::protobuf::internal::kEmptyString) {
    delete texture_;
  }
  if (texture) {
    set_has_texture();
    texture_ = texture;
  } else {
    clear_has_texture();
    texture_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// optional int32 duration_ms = 3 [default = 1000];
inline bool Animation::has_duration_ms() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void Animation::set_has_duration_ms() {
  _has_bits_[0] |= 0x00000004u;
}
inline void Animation::clear_has_duration_ms() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void Animation::clear_duration_ms() {
  duration_ms_ = 1000;
  clear_has_duration_ms();
}
inline ::google::protobuf::int32 Animation::duration_ms() const {
  return duration_ms_;
}
inline void Animation::set_duration_ms(::google::protobuf::int32 value) {
  set_has_duration_ms();
  duration_ms_ = value;
}

// repeated .rogue.sprite.TextureRect frame = 4;
inline int Animation::frame_size() const {
  return frame_.size();
}
inline void Animation::clear_frame() {
  frame_.Clear();
}
inline const ::rogue::sprite::TextureRect& Animation::frame(int index) const {
  return frame_.Get(index);
}
inline ::rogue::sprite::TextureRect* Animation::mutable_frame(int index) {
  return frame_.Mutable(index);
}
inline ::rogue::sprite::TextureRect* Animation::add_frame() {
  return frame_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::rogue::sprite::TextureRect >&
Animation::frame() const {
  return frame_;
}
inline ::google::protobuf::RepeatedPtrField< ::rogue::sprite::TextureRect >*
Animation::mutable_frame() {
  return &frame_;
}

// optional bool looping = 5 [default = false];
inline bool Animation::has_looping() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void Animation::set_has_looping() {
  _has_bits_[0] |= 0x00000010u;
}
inline void Animation::clear_has_looping() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void Animation::clear_looping() {
  looping_ = false;
  clear_has_looping();
}
inline bool Animation::looping() const {
  return looping_;
}
inline void Animation::set_looping(bool value) {
  set_has_looping();
  looping_ = value;
}

// -------------------------------------------------------------------

// Animations

// repeated .rogue.animation_config.Animation animation = 1;
inline int Animations::animation_size() const {
  return animation_.size();
}
inline void Animations::clear_animation() {
  animation_.Clear();
}
inline const ::rogue::animation_config::Animation& Animations::animation(int index) const {
  return animation_.Get(index);
}
inline ::rogue::animation_config::Animation* Animations::mutable_animation(int index) {
  return animation_.Mutable(index);
}
inline ::rogue::animation_config::Animation* Animations::add_animation() {
  return animation_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::rogue::animation_config::Animation >&
Animations::animation() const {
  return animation_;
}
inline ::google::protobuf::RepeatedPtrField< ::rogue::animation_config::Animation >*
Animations::mutable_animation() {
  return &animation_;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace animation_config
}  // namespace rogue

#ifndef SWIG
namespace google {
namespace protobuf {

template <>
inline const EnumDescriptor* GetEnumDescriptor< ::rogue::animation_config::AnimationId>() {
  return ::rogue::animation_config::AnimationId_descriptor();
}

}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_animation_5fconfig_2eproto__INCLUDED
