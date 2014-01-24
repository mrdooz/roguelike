// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: animation_config.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "animation_config.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace rogue {
namespace animation_config {

namespace {

const ::google::protobuf::Descriptor* Frame_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  Frame_reflection_ = NULL;
const ::google::protobuf::Descriptor* Animation_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  Animation_reflection_ = NULL;
const ::google::protobuf::Descriptor* Animations_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  Animations_reflection_ = NULL;
const ::google::protobuf::EnumDescriptor* AnimationId_descriptor_ = NULL;

}  // namespace


void protobuf_AssignDesc_animation_5fconfig_2eproto() {
  protobuf_AddDesc_animation_5fconfig_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "animation_config.proto");
  GOOGLE_CHECK(file != NULL);
  Frame_descriptor_ = file->message_type(0);
  static const int Frame_offsets_[2] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Frame, weight_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Frame, texture_rect_),
  };
  Frame_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      Frame_descriptor_,
      Frame::default_instance_,
      Frame_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Frame, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Frame, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(Frame));
  Animation_descriptor_ = file->message_type(1);
  static const int Animation_offsets_[6] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Animation, id_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Animation, texture_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Animation, duration_ms_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Animation, frame_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Animation, looping_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Animation, name_),
  };
  Animation_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      Animation_descriptor_,
      Animation::default_instance_,
      Animation_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Animation, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Animation, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(Animation));
  Animations_descriptor_ = file->message_type(2);
  static const int Animations_offsets_[1] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Animations, animation_),
  };
  Animations_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      Animations_descriptor_,
      Animations::default_instance_,
      Animations_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Animations, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Animations, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(Animations));
  AnimationId_descriptor_ = file->enum_type(0);
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_animation_5fconfig_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    Frame_descriptor_, &Frame::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    Animation_descriptor_, &Animation::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    Animations_descriptor_, &Animations::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_animation_5fconfig_2eproto() {
  delete Frame::default_instance_;
  delete Frame_reflection_;
  delete Animation::default_instance_;
  delete Animation_reflection_;
  delete Animations::default_instance_;
  delete Animations_reflection_;
}

void protobuf_AddDesc_animation_5fconfig_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::rogue::sprite::protobuf_AddDesc_sprite_2eproto();
  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\026animation_config.proto\022\026rogue.animatio"
    "n_config\032\014sprite.proto\"K\n\005Frame\022\021\n\006weigh"
    "t\030\001 \001(\005:\0011\022/\n\014texture_rect\030\002 \001(\0132\031.rogue"
    ".sprite.TextureRect\"\274\001\n\tAnimation\022/\n\002id\030"
    "\001 \001(\0162#.rogue.animation_config.Animation"
    "Id\022\017\n\007texture\030\002 \001(\t\022\031\n\013duration_ms\030\003 \001(\005"
    ":\0041000\022,\n\005frame\030\004 \003(\0132\035.rogue.animation_"
    "config.Frame\022\026\n\007looping\030\005 \001(\010:\005false\022\014\n\004"
    "name\030\006 \001(\t\"B\n\nAnimations\0224\n\tanimation\030\001 "
    "\003(\0132!.rogue.animation_config.Animation*o"
    "\n\013AnimationId\022\010\n\004None\020\000\022\t\n\005Blood\020\001\022\017\n\013Ar"
    "caneBlast\020\002\022\022\n\016LightningBolt2\020\003\022\022\n\016Light"
    "ningBolt3\020\004\022\022\n\016LightningBolt4\020\005", 511);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "animation_config.proto", &protobuf_RegisterTypes);
  Frame::default_instance_ = new Frame();
  Animation::default_instance_ = new Animation();
  Animations::default_instance_ = new Animations();
  Frame::default_instance_->InitAsDefaultInstance();
  Animation::default_instance_->InitAsDefaultInstance();
  Animations::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_animation_5fconfig_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_animation_5fconfig_2eproto {
  StaticDescriptorInitializer_animation_5fconfig_2eproto() {
    protobuf_AddDesc_animation_5fconfig_2eproto();
  }
} static_descriptor_initializer_animation_5fconfig_2eproto_;
const ::google::protobuf::EnumDescriptor* AnimationId_descriptor() {
  protobuf_AssignDescriptorsOnce();
  return AnimationId_descriptor_;
}
bool AnimationId_IsValid(int value) {
  switch(value) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
      return true;
    default:
      return false;
  }
}


// ===================================================================

#ifndef _MSC_VER
const int Frame::kWeightFieldNumber;
const int Frame::kTextureRectFieldNumber;
#endif  // !_MSC_VER

Frame::Frame()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void Frame::InitAsDefaultInstance() {
  texture_rect_ = const_cast< ::rogue::sprite::TextureRect*>(&::rogue::sprite::TextureRect::default_instance());
}

Frame::Frame(const Frame& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void Frame::SharedCtor() {
  _cached_size_ = 0;
  weight_ = 1;
  texture_rect_ = NULL;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

Frame::~Frame() {
  SharedDtor();
}

void Frame::SharedDtor() {
  if (this != default_instance_) {
    delete texture_rect_;
  }
}

void Frame::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* Frame::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return Frame_descriptor_;
}

const Frame& Frame::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_animation_5fconfig_2eproto();
  return *default_instance_;
}

Frame* Frame::default_instance_ = NULL;

Frame* Frame::New() const {
  return new Frame;
}

void Frame::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    weight_ = 1;
    if (has_texture_rect()) {
      if (texture_rect_ != NULL) texture_rect_->::rogue::sprite::TextureRect::Clear();
    }
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool Frame::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional int32 weight = 1 [default = 1];
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &weight_)));
          set_has_weight();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(18)) goto parse_texture_rect;
        break;
      }

      // optional .rogue.sprite.TextureRect texture_rect = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_texture_rect:
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
               input, mutable_texture_rect()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectAtEnd()) return true;
        break;
      }

      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void Frame::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // optional int32 weight = 1 [default = 1];
  if (has_weight()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(1, this->weight(), output);
  }

  // optional .rogue.sprite.TextureRect texture_rect = 2;
  if (has_texture_rect()) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      2, this->texture_rect(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* Frame::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // optional int32 weight = 1 [default = 1];
  if (has_weight()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(1, this->weight(), target);
  }

  // optional .rogue.sprite.TextureRect texture_rect = 2;
  if (has_texture_rect()) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteMessageNoVirtualToArray(
        2, this->texture_rect(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int Frame::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // optional int32 weight = 1 [default = 1];
    if (has_weight()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->weight());
    }

    // optional .rogue.sprite.TextureRect texture_rect = 2;
    if (has_texture_rect()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
          this->texture_rect());
    }

  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void Frame::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const Frame* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const Frame*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void Frame::MergeFrom(const Frame& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_weight()) {
      set_weight(from.weight());
    }
    if (from.has_texture_rect()) {
      mutable_texture_rect()->::rogue::sprite::TextureRect::MergeFrom(from.texture_rect());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void Frame::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void Frame::CopyFrom(const Frame& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool Frame::IsInitialized() const {

  return true;
}

void Frame::Swap(Frame* other) {
  if (other != this) {
    std::swap(weight_, other->weight_);
    std::swap(texture_rect_, other->texture_rect_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata Frame::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = Frame_descriptor_;
  metadata.reflection = Frame_reflection_;
  return metadata;
}


// ===================================================================

#ifndef _MSC_VER
const int Animation::kIdFieldNumber;
const int Animation::kTextureFieldNumber;
const int Animation::kDurationMsFieldNumber;
const int Animation::kFrameFieldNumber;
const int Animation::kLoopingFieldNumber;
const int Animation::kNameFieldNumber;
#endif  // !_MSC_VER

Animation::Animation()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void Animation::InitAsDefaultInstance() {
}

Animation::Animation(const Animation& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void Animation::SharedCtor() {
  _cached_size_ = 0;
  id_ = 0;
  texture_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  duration_ms_ = 1000;
  looping_ = false;
  name_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

Animation::~Animation() {
  SharedDtor();
}

void Animation::SharedDtor() {
  if (texture_ != &::google::protobuf::internal::kEmptyString) {
    delete texture_;
  }
  if (name_ != &::google::protobuf::internal::kEmptyString) {
    delete name_;
  }
  if (this != default_instance_) {
  }
}

void Animation::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* Animation::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return Animation_descriptor_;
}

const Animation& Animation::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_animation_5fconfig_2eproto();
  return *default_instance_;
}

Animation* Animation::default_instance_ = NULL;

Animation* Animation::New() const {
  return new Animation;
}

void Animation::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    id_ = 0;
    if (has_texture()) {
      if (texture_ != &::google::protobuf::internal::kEmptyString) {
        texture_->clear();
      }
    }
    duration_ms_ = 1000;
    looping_ = false;
    if (has_name()) {
      if (name_ != &::google::protobuf::internal::kEmptyString) {
        name_->clear();
      }
    }
  }
  frame_.Clear();
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool Animation::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional .rogue.animation_config.AnimationId id = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          int value;
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   int, ::google::protobuf::internal::WireFormatLite::TYPE_ENUM>(
                 input, &value)));
          if (::rogue::animation_config::AnimationId_IsValid(value)) {
            set_id(static_cast< ::rogue::animation_config::AnimationId >(value));
          } else {
            mutable_unknown_fields()->AddVarint(1, value);
          }
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(18)) goto parse_texture;
        break;
      }

      // optional string texture = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_texture:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_texture()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->texture().data(), this->texture().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(24)) goto parse_duration_ms;
        break;
      }

      // optional int32 duration_ms = 3 [default = 1000];
      case 3: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_duration_ms:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &duration_ms_)));
          set_has_duration_ms();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(34)) goto parse_frame;
        break;
      }

      // repeated .rogue.animation_config.Frame frame = 4;
      case 4: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_frame:
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
                input, add_frame()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(34)) goto parse_frame;
        if (input->ExpectTag(40)) goto parse_looping;
        break;
      }

      // optional bool looping = 5 [default = false];
      case 5: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_looping:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   bool, ::google::protobuf::internal::WireFormatLite::TYPE_BOOL>(
                 input, &looping_)));
          set_has_looping();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(50)) goto parse_name;
        break;
      }

      // optional string name = 6;
      case 6: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_name:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_name()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->name().data(), this->name().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectAtEnd()) return true;
        break;
      }

      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void Animation::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // optional .rogue.animation_config.AnimationId id = 1;
  if (has_id()) {
    ::google::protobuf::internal::WireFormatLite::WriteEnum(
      1, this->id(), output);
  }

  // optional string texture = 2;
  if (has_texture()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->texture().data(), this->texture().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      2, this->texture(), output);
  }

  // optional int32 duration_ms = 3 [default = 1000];
  if (has_duration_ms()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(3, this->duration_ms(), output);
  }

  // repeated .rogue.animation_config.Frame frame = 4;
  for (int i = 0; i < this->frame_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      4, this->frame(i), output);
  }

  // optional bool looping = 5 [default = false];
  if (has_looping()) {
    ::google::protobuf::internal::WireFormatLite::WriteBool(5, this->looping(), output);
  }

  // optional string name = 6;
  if (has_name()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->name().data(), this->name().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      6, this->name(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* Animation::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // optional .rogue.animation_config.AnimationId id = 1;
  if (has_id()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteEnumToArray(
      1, this->id(), target);
  }

  // optional string texture = 2;
  if (has_texture()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->texture().data(), this->texture().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        2, this->texture(), target);
  }

  // optional int32 duration_ms = 3 [default = 1000];
  if (has_duration_ms()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(3, this->duration_ms(), target);
  }

  // repeated .rogue.animation_config.Frame frame = 4;
  for (int i = 0; i < this->frame_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteMessageNoVirtualToArray(
        4, this->frame(i), target);
  }

  // optional bool looping = 5 [default = false];
  if (has_looping()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteBoolToArray(5, this->looping(), target);
  }

  // optional string name = 6;
  if (has_name()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->name().data(), this->name().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        6, this->name(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int Animation::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // optional .rogue.animation_config.AnimationId id = 1;
    if (has_id()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::EnumSize(this->id());
    }

    // optional string texture = 2;
    if (has_texture()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->texture());
    }

    // optional int32 duration_ms = 3 [default = 1000];
    if (has_duration_ms()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->duration_ms());
    }

    // optional bool looping = 5 [default = false];
    if (has_looping()) {
      total_size += 1 + 1;
    }

    // optional string name = 6;
    if (has_name()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->name());
    }

  }
  // repeated .rogue.animation_config.Frame frame = 4;
  total_size += 1 * this->frame_size();
  for (int i = 0; i < this->frame_size(); i++) {
    total_size +=
      ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
        this->frame(i));
  }

  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void Animation::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const Animation* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const Animation*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void Animation::MergeFrom(const Animation& from) {
  GOOGLE_CHECK_NE(&from, this);
  frame_.MergeFrom(from.frame_);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_id()) {
      set_id(from.id());
    }
    if (from.has_texture()) {
      set_texture(from.texture());
    }
    if (from.has_duration_ms()) {
      set_duration_ms(from.duration_ms());
    }
    if (from.has_looping()) {
      set_looping(from.looping());
    }
    if (from.has_name()) {
      set_name(from.name());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void Animation::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void Animation::CopyFrom(const Animation& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool Animation::IsInitialized() const {

  return true;
}

void Animation::Swap(Animation* other) {
  if (other != this) {
    std::swap(id_, other->id_);
    std::swap(texture_, other->texture_);
    std::swap(duration_ms_, other->duration_ms_);
    frame_.Swap(&other->frame_);
    std::swap(looping_, other->looping_);
    std::swap(name_, other->name_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata Animation::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = Animation_descriptor_;
  metadata.reflection = Animation_reflection_;
  return metadata;
}


// ===================================================================

#ifndef _MSC_VER
const int Animations::kAnimationFieldNumber;
#endif  // !_MSC_VER

Animations::Animations()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void Animations::InitAsDefaultInstance() {
}

Animations::Animations(const Animations& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void Animations::SharedCtor() {
  _cached_size_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

Animations::~Animations() {
  SharedDtor();
}

void Animations::SharedDtor() {
  if (this != default_instance_) {
  }
}

void Animations::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* Animations::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return Animations_descriptor_;
}

const Animations& Animations::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_animation_5fconfig_2eproto();
  return *default_instance_;
}

Animations* Animations::default_instance_ = NULL;

Animations* Animations::New() const {
  return new Animations;
}

void Animations::Clear() {
  animation_.Clear();
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool Animations::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // repeated .rogue.animation_config.Animation animation = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_animation:
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
                input, add_animation()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(10)) goto parse_animation;
        if (input->ExpectAtEnd()) return true;
        break;
      }

      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void Animations::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // repeated .rogue.animation_config.Animation animation = 1;
  for (int i = 0; i < this->animation_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      1, this->animation(i), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* Animations::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // repeated .rogue.animation_config.Animation animation = 1;
  for (int i = 0; i < this->animation_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteMessageNoVirtualToArray(
        1, this->animation(i), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int Animations::ByteSize() const {
  int total_size = 0;

  // repeated .rogue.animation_config.Animation animation = 1;
  total_size += 1 * this->animation_size();
  for (int i = 0; i < this->animation_size(); i++) {
    total_size +=
      ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
        this->animation(i));
  }

  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void Animations::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const Animations* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const Animations*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void Animations::MergeFrom(const Animations& from) {
  GOOGLE_CHECK_NE(&from, this);
  animation_.MergeFrom(from.animation_);
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void Animations::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void Animations::CopyFrom(const Animations& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool Animations::IsInitialized() const {

  return true;
}

void Animations::Swap(Animations* other) {
  if (other != this) {
    animation_.Swap(&other->animation_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata Animations::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = Animations_descriptor_;
  metadata.reflection = Animations_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace animation_config
}  // namespace rogue

// @@protoc_insertion_point(global_scope)
