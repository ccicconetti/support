#include "versionutils.h"

#include "version.h"

namespace uiiit {
namespace support {

std::string version() noexcept {
  return GIT_DESCRIBE;
}

} // namespace support
} // namespace uiiit
