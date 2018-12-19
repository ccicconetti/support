#pragma once

#include <string>
#include <vector>

namespace uiiit {
namespace support {

/**
 * Helper class to format two-dimensional string data in a Mattermost table.
 * The header line is mandatory.
 * If the table is empty it is rendered as an empty string.
 */
class MmTable
{

 public:
  /**
   * \param Number of colums in the table. Cannot be changed afterwards.
   *
   * \throw std::runtime_error if aNumCols is 0.
   */
  explicit MmTable(const size_t aNumCols);

  /**
   * Fill a given cell. Resize the table if necessary.
   *
   * \param aRow The row index, 0-based.
   * \param aCol The column index, 0-based.
   * \param aValue The cell value, can be empty.
   *
   * \throw std::runtime_error if aCol exceeds the number of columns specified
   * in the ctor.
   */
  void
  operator()(const size_t aRow, const size_t aCol, const std::string& aValue);

  std::string toString() const;

 private:
  const size_t             theNumCols;
  std::vector<std::string> theTable;
};

} // namespace support
} // namespace uiiit
