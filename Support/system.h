#pragma once

#include <string>

namespace uiiit {
namespace support {

/**
 * Provides with system information.
 *
 * Class is singleton.
 * Method access is not thread-safe.
 * Acquisition of system information is lazy.
 */
class System
{
 public:
  static System& instance();

  std::string hostName() noexcept;
  std::string cpuName() noexcept;

 private:
  explicit System();
  std::string cpuNameLinux() noexcept;
  std::string cpuNameMac() noexcept;

 private:
  std::string theHostName;
  std::string theCpuName;
};

} // namespace support
} // namespace uiiit
