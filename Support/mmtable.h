/*
 ___ ___ __     __ ____________
|   |   |  |   |__|__|__   ___/  Ubiquitout Internet @ IIT-CNR
|   |   |  |  /__/  /  /  /      C++ support library
|   |   |  |/__/  /   /  /       https://bitbucket.org/ccicconetti/support/
|_______|__|__/__/   /__/

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
Copyright (c) 2018 Claudio Cicconetti <https://about.me/ccicconetti>

Permission is hereby  granted, free of charge, to any  person obtaining a copy
of this software and associated  documentation files (the "Software"), to deal
in the Software  without restriction, including without  limitation the rights
to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

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
