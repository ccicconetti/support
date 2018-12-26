#pragma once

#include "Support/wait.h"

#include <string>

#include <boost/filesystem.hpp>

#define WAIT_FOR(pred, time)                                                   \
  ASSERT_TRUE(support::waitFor<bool>(pred, true, time))

namespace uiiit {
namespace support {

struct TempDirRaii {
  static const std::string& name() {
    static const std::string myName("REMOVE_ME");
    return myName;
  }

  static const boost::filesystem::path& path() {
    static const boost::filesystem::path myPath(
        boost::filesystem::current_path() / name());
    return myPath;
  }

  explicit TempDirRaii() {
    boost::filesystem::remove_all(name());
    boost::filesystem::create_directory(name());
  }

  ~TempDirRaii() {
    boost::filesystem::remove_all(name());
  }
};

} // namespace support
} // namespace uiiit
