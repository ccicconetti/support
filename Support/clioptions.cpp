#include "clioptions.h"

#include "Support/versionutils.h"

#include <iostream>

namespace uiiit {
namespace support {

CliOptions::CliOptions(int                                          aArgc,
                       char**                                       aArgv,
                       boost::program_options::options_description& aDesc)
    : theVarMap()
    , theArgc(aArgc)
    , theArgv(aArgv)
    , theDesc(aDesc) {
  // clang-format off
  theDesc.add_options()
    ("help,h", "produce help message")
    ("version,v", "print program version")
    ;
  // clang-format on
}

void CliOptions::parse() {
  boost::program_options::store(
      boost::program_options::parse_command_line(theArgc, theArgv, theDesc),
      theVarMap);
  boost::program_options::notify(theVarMap);

  if (theVarMap.count("help") > 0) {
    std::cout << theDesc << std::endl;
    throw CliExit();
  }

  if (theVarMap.count("version") > 0) {
    std::cout << version() << std::endl;
    throw CliExit();
  }
}

const boost::program_options::variables_map& CliOptions::varMap() noexcept {
  return theVarMap;
}

} // namespace support
} // namespace uiiit
