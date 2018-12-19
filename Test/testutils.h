#pragma once

#include <string>

#include <boost/filesystem.hpp>

namespace uiiit {
namespace test {

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

} // namespace test
} // namespace uiiit
