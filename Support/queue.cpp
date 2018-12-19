#include "queue.h"

namespace uiiit {
namespace support {

QueueClosed::QueueClosed()
    : std::runtime_error("Queue closed") {
}

} // namespace support
} // namespace uiiit
