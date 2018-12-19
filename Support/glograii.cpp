#include "glograii.h"

#include <glog/logging.h>

namespace uiiit {
namespace support {

GlogRaii::GlogRaii(const char* argv0) {
  ::google::InitGoogleLogging(argv0);
  ::google::LogToStderr();
}

GlogRaii::~GlogRaii() {
  ::google::ShutdownGoogleLogging();
}

} // namespace support
} // namespace uiiit
