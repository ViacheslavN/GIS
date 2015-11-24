// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: google/protobuf/util/internal/testdata/timestamp_duration.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "google/protobuf/util/internal/testdata/timestamp_duration.pb.h"

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

namespace google {
namespace protobuf {
namespace testing {
namespace timestampduration {

namespace {

const ::google::protobuf::Descriptor* TimestampDuration_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  TimestampDuration_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_google_2fprotobuf_2futil_2finternal_2ftestdata_2ftimestamp_5fduration_2eproto() {
  protobuf_AddDesc_google_2fprotobuf_2futil_2finternal_2ftestdata_2ftimestamp_5fduration_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "google/protobuf/util/internal/testdata/timestamp_duration.proto");
  GOOGLE_CHECK(file != NULL);
  TimestampDuration_descriptor_ = file->message_type(0);
  static const int TimestampDuration_offsets_[2] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(TimestampDuration, ts_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(TimestampDuration, dur_),
  };
  TimestampDuration_reflection_ =
    ::google::protobuf::internal::GeneratedMessageReflection::NewGeneratedMessageReflection(
      TimestampDuration_descriptor_,
      TimestampDuration::default_instance_,
      TimestampDuration_offsets_,
      -1,
      -1,
      -1,
      sizeof(TimestampDuration),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(TimestampDuration, _internal_metadata_),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(TimestampDuration, _is_default_instance_));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_google_2fprotobuf_2futil_2finternal_2ftestdata_2ftimestamp_5fduration_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
      TimestampDuration_descriptor_, &TimestampDuration::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_google_2fprotobuf_2futil_2finternal_2ftestdata_2ftimestamp_5fduration_2eproto() {
  delete TimestampDuration::default_instance_;
  delete TimestampDuration_reflection_;
}

void protobuf_AddDesc_google_2fprotobuf_2futil_2finternal_2ftestdata_2ftimestamp_5fduration_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::protobuf_AddDesc_google_2fprotobuf_2ftimestamp_2eproto();
  ::google::protobuf::protobuf_AddDesc_google_2fprotobuf_2fduration_2eproto();
  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\?google/protobuf/util/internal/testdata"
    "/timestamp_duration.proto\022)google.protob"
    "uf.testing.timestampduration\032\037google/pro"
    "tobuf/timestamp.proto\032\036google/protobuf/d"
    "uration.proto\"c\n\021TimestampDuration\022&\n\002ts"
    "\030\001 \001(\0132\032.google.protobuf.Timestamp\022&\n\003du"
    "r\030\002 \001(\0132\031.google.protobuf.Duration2\222\001\n\013T"
    "estService\022\202\001\n\004Call\022<.google.protobuf.te"
    "sting.timestampduration.TimestampDuratio"
    "n\032<.google.protobuf.testing.timestampdur"
    "ation.TimestampDurationB/\n-com.google.pr"
    "otobuf.testing.timestampdurationb\006proto3", 480);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "google/protobuf/util/internal/testdata/timestamp_duration.proto", &protobuf_RegisterTypes);
  TimestampDuration::default_instance_ = new TimestampDuration();
  TimestampDuration::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_google_2fprotobuf_2futil_2finternal_2ftestdata_2ftimestamp_5fduration_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_google_2fprotobuf_2futil_2finternal_2ftestdata_2ftimestamp_5fduration_2eproto {
  StaticDescriptorInitializer_google_2fprotobuf_2futil_2finternal_2ftestdata_2ftimestamp_5fduration_2eproto() {
    protobuf_AddDesc_google_2fprotobuf_2futil_2finternal_2ftestdata_2ftimestamp_5fduration_2eproto();
  }
} static_descriptor_initializer_google_2fprotobuf_2futil_2finternal_2ftestdata_2ftimestamp_5fduration_2eproto_;

namespace {

static void MergeFromFail(int line) GOOGLE_ATTRIBUTE_COLD;
static void MergeFromFail(int line) {
  GOOGLE_CHECK(false) << __FILE__ << ":" << line;
}

}  // namespace


// ===================================================================

#if !defined(_MSC_VER) || _MSC_VER >= 1900
const int TimestampDuration::kTsFieldNumber;
const int TimestampDuration::kDurFieldNumber;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

TimestampDuration::TimestampDuration()
  : ::google::protobuf::Message(), _internal_metadata_(NULL) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:google.protobuf.testing.timestampduration.TimestampDuration)
}

void TimestampDuration::InitAsDefaultInstance() {
  _is_default_instance_ = true;
  ts_ = const_cast< ::google::protobuf::Timestamp*>(&::google::protobuf::Timestamp::default_instance());
  dur_ = const_cast< ::google::protobuf::Duration*>(&::google::protobuf::Duration::default_instance());
}

TimestampDuration::TimestampDuration(const TimestampDuration& from)
  : ::google::protobuf::Message(),
    _internal_metadata_(NULL) {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:google.protobuf.testing.timestampduration.TimestampDuration)
}

void TimestampDuration::SharedCtor() {
    _is_default_instance_ = false;
  _cached_size_ = 0;
  ts_ = NULL;
  dur_ = NULL;
}

TimestampDuration::~TimestampDuration() {
  // @@protoc_insertion_point(destructor:google.protobuf.testing.timestampduration.TimestampDuration)
  SharedDtor();
}

void TimestampDuration::SharedDtor() {
  if (this != default_instance_) {
    delete ts_;
    delete dur_;
  }
}

void TimestampDuration::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* TimestampDuration::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return TimestampDuration_descriptor_;
}

const TimestampDuration& TimestampDuration::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_google_2fprotobuf_2futil_2finternal_2ftestdata_2ftimestamp_5fduration_2eproto();
  return *default_instance_;
}

TimestampDuration* TimestampDuration::default_instance_ = NULL;

TimestampDuration* TimestampDuration::New(::google::protobuf::Arena* arena) const {
  TimestampDuration* n = new TimestampDuration;
  if (arena != NULL) {
    arena->Own(n);
  }
  return n;
}

void TimestampDuration::Clear() {
  if (GetArenaNoVirtual() == NULL && ts_ != NULL) delete ts_;
  ts_ = NULL;
  if (GetArenaNoVirtual() == NULL && dur_ != NULL) delete dur_;
  dur_ = NULL;
}

bool TimestampDuration::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:google.protobuf.testing.timestampduration.TimestampDuration)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoff(127);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional .google.protobuf.Timestamp ts = 1;
      case 1: {
        if (tag == 10) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
               input, mutable_ts()));
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(18)) goto parse_dur;
        break;
      }

      // optional .google.protobuf.Duration dur = 2;
      case 2: {
        if (tag == 18) {
         parse_dur:
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
               input, mutable_dur()));
        } else {
          goto handle_unusual;
        }
        if (input->ExpectAtEnd()) goto success;
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0 ||
            ::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormatLite::SkipField(input, tag));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:google.protobuf.testing.timestampduration.TimestampDuration)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:google.protobuf.testing.timestampduration.TimestampDuration)
  return false;
#undef DO_
}

void TimestampDuration::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:google.protobuf.testing.timestampduration.TimestampDuration)
  // optional .google.protobuf.Timestamp ts = 1;
  if (this->has_ts()) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      1, *this->ts_, output);
  }

  // optional .google.protobuf.Duration dur = 2;
  if (this->has_dur()) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      2, *this->dur_, output);
  }

  // @@protoc_insertion_point(serialize_end:google.protobuf.testing.timestampduration.TimestampDuration)
}

::google::protobuf::uint8* TimestampDuration::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // @@protoc_insertion_point(serialize_to_array_start:google.protobuf.testing.timestampduration.TimestampDuration)
  // optional .google.protobuf.Timestamp ts = 1;
  if (this->has_ts()) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteMessageNoVirtualToArray(
        1, *this->ts_, target);
  }

  // optional .google.protobuf.Duration dur = 2;
  if (this->has_dur()) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteMessageNoVirtualToArray(
        2, *this->dur_, target);
  }

  // @@protoc_insertion_point(serialize_to_array_end:google.protobuf.testing.timestampduration.TimestampDuration)
  return target;
}

int TimestampDuration::ByteSize() const {
  int total_size = 0;

  // optional .google.protobuf.Timestamp ts = 1;
  if (this->has_ts()) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
        *this->ts_);
  }

  // optional .google.protobuf.Duration dur = 2;
  if (this->has_dur()) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
        *this->dur_);
  }

  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void TimestampDuration::MergeFrom(const ::google::protobuf::Message& from) {
  if (GOOGLE_PREDICT_FALSE(&from == this)) MergeFromFail(__LINE__);
  const TimestampDuration* source = 
      ::google::protobuf::internal::DynamicCastToGenerated<const TimestampDuration>(
          &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void TimestampDuration::MergeFrom(const TimestampDuration& from) {
  if (GOOGLE_PREDICT_FALSE(&from == this)) MergeFromFail(__LINE__);
  if (from.has_ts()) {
    mutable_ts()->::google::protobuf::Timestamp::MergeFrom(from.ts());
  }
  if (from.has_dur()) {
    mutable_dur()->::google::protobuf::Duration::MergeFrom(from.dur());
  }
}

void TimestampDuration::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void TimestampDuration::CopyFrom(const TimestampDuration& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool TimestampDuration::IsInitialized() const {

  return true;
}

void TimestampDuration::Swap(TimestampDuration* other) {
  if (other == this) return;
  InternalSwap(other);
}
void TimestampDuration::InternalSwap(TimestampDuration* other) {
  std::swap(ts_, other->ts_);
  std::swap(dur_, other->dur_);
  _internal_metadata_.Swap(&other->_internal_metadata_);
  std::swap(_cached_size_, other->_cached_size_);
}

::google::protobuf::Metadata TimestampDuration::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = TimestampDuration_descriptor_;
  metadata.reflection = TimestampDuration_reflection_;
  return metadata;
}

#if PROTOBUF_INLINE_NOT_IN_HEADERS
// TimestampDuration

// optional .google.protobuf.Timestamp ts = 1;
bool TimestampDuration::has_ts() const {
  return !_is_default_instance_ && ts_ != NULL;
}
void TimestampDuration::clear_ts() {
  if (GetArenaNoVirtual() == NULL && ts_ != NULL) delete ts_;
  ts_ = NULL;
}
const ::google::protobuf::Timestamp& TimestampDuration::ts() const {
  // @@protoc_insertion_point(field_get:google.protobuf.testing.timestampduration.TimestampDuration.ts)
  return ts_ != NULL ? *ts_ : *default_instance_->ts_;
}
::google::protobuf::Timestamp* TimestampDuration::mutable_ts() {
  
  if (ts_ == NULL) {
    ts_ = new ::google::protobuf::Timestamp;
  }
  // @@protoc_insertion_point(field_mutable:google.protobuf.testing.timestampduration.TimestampDuration.ts)
  return ts_;
}
::google::protobuf::Timestamp* TimestampDuration::release_ts() {
  
  ::google::protobuf::Timestamp* temp = ts_;
  ts_ = NULL;
  return temp;
}
void TimestampDuration::set_allocated_ts(::google::protobuf::Timestamp* ts) {
  delete ts_;
  ts_ = ts;
  if (ts) {
    
  } else {
    
  }
  // @@protoc_insertion_point(field_set_allocated:google.protobuf.testing.timestampduration.TimestampDuration.ts)
}

// optional .google.protobuf.Duration dur = 2;
bool TimestampDuration::has_dur() const {
  return !_is_default_instance_ && dur_ != NULL;
}
void TimestampDuration::clear_dur() {
  if (GetArenaNoVirtual() == NULL && dur_ != NULL) delete dur_;
  dur_ = NULL;
}
const ::google::protobuf::Duration& TimestampDuration::dur() const {
  // @@protoc_insertion_point(field_get:google.protobuf.testing.timestampduration.TimestampDuration.dur)
  return dur_ != NULL ? *dur_ : *default_instance_->dur_;
}
::google::protobuf::Duration* TimestampDuration::mutable_dur() {
  
  if (dur_ == NULL) {
    dur_ = new ::google::protobuf::Duration;
  }
  // @@protoc_insertion_point(field_mutable:google.protobuf.testing.timestampduration.TimestampDuration.dur)
  return dur_;
}
::google::protobuf::Duration* TimestampDuration::release_dur() {
  
  ::google::protobuf::Duration* temp = dur_;
  dur_ = NULL;
  return temp;
}
void TimestampDuration::set_allocated_dur(::google::protobuf::Duration* dur) {
  delete dur_;
  dur_ = dur;
  if (dur) {
    
  } else {
    
  }
  // @@protoc_insertion_point(field_set_allocated:google.protobuf.testing.timestampduration.TimestampDuration.dur)
}

#endif  // PROTOBUF_INLINE_NOT_IN_HEADERS

// @@protoc_insertion_point(namespace_scope)

}  // namespace timestampduration
}  // namespace testing
}  // namespace protobuf
}  // namespace google

// @@protoc_insertion_point(global_scope)
