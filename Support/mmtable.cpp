#include "mmtable.h"

#include <cassert>
#include <sstream>
#include <stdexcept>

namespace uiiit {
namespace support {

MmTable::MmTable(const size_t aNumCols)
    : theNumCols(aNumCols)
    , theTable() {
  if (aNumCols == 0) {
    throw std::runtime_error("Invalid number of columns: 0");
  }
}

void MmTable::
     operator()(const size_t aRow, const size_t aCol, const std::string& aValue) {
  if (aCol >= theNumCols) {
    throw std::runtime_error("Invalid column index: " + std::to_string(aCol) +
                             ", maximum is " + std::to_string(theNumCols - 1));
  }
  const auto myIndex = aRow * theNumCols + aCol;
  theTable.resize(myIndex + 1);
  theTable.at(myIndex) = aValue;
}

std::string MmTable::toString() const {
  if (theTable.empty()) {
    return "";
  }
  const auto        myNumRows = 1 + (theTable.size() - 1) / theNumCols;
  std::stringstream myStream;
  for (size_t myRow = 0; myRow < myNumRows; ++myRow) {
    for (size_t myCol = 0; myCol < theNumCols; myCol++) {
      if (myCol == 0) {
        myStream << '|';
      }
      const auto myIndex = myRow * theNumCols + myCol;
      const auto myValue =
          myIndex < theTable.size() ? theTable.at(myIndex) : std::string();
      myStream << ' ' << myValue << ' ' << '|';
    }
    myStream << '\n';

    // print the header/body seperation line
    if (myRow == 0) {
      for (size_t myCol = 0; myCol < theNumCols; myCol++) {
        if (myCol == 0) {
          myStream << '|';
        }
        myStream << " - |";
      }
      myStream << '\n';
    }
  }
  // if the table is header-only (= 1 row) then add an empty row
  assert(myNumRows > 0);
  if (myNumRows == 1) {
    for (size_t myCol = 0; myCol < theNumCols; myCol++) {
      if (myCol == 0) {
        myStream << '|';
      }
      myStream << " |";
    }
    myStream << '\n';
  }
  return myStream.str();
}

} // namespace support
} // namespace uiiit
