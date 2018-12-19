#pragma once

#include <boost/program_options.hpp>

#include <stdexcept>

namespace uiiit {
namespace support {

//! Raised to trigger a clean quit of the application after parsing CLI options.
struct CliExit final : public std::runtime_error {
  explicit CliExit()
      : std::runtime_error("") {
  }
};

/**
 * Basic wrapper to boost::program_options functions to parse command-line
 * arguments. Must be derived.
 *
 * Add --help and --version default arguments.
 */
class CliOptions
{
 public:
  virtual ~CliOptions() {
  }

  //! \return the map of variables parsed from CLI.
  const boost::program_options::variables_map& varMap() noexcept;

 protected:
  CliOptions(int                                          argc,
             char**                                       argv,
             boost::program_options::options_description& aDesc);

  /**
   * Perform actual parsing of the command-line arguments.
   * Must be called by the derived classes.
   */
  void parse();

 protected:
  boost::program_options::variables_map        theVarMap;
  int                                          theArgc;
  char**                                       theArgv;
  boost::program_options::options_description& theDesc;
};

} // namespace support
} // namespace uiiit
