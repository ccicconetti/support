/*
 ___ ___ __     __ ____________
|   |   |  |   |__|__|__   ___/  Ubiquitout Internet @ IIT-CNR
|   |   |  |  /__/  /  /  /      C++ support library
|   |   |  |/__/  /   /  /       https://github.com/ccicconetti/support/
|_______|__|__/__/   /__/

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
Copyright (c) 2019 Claudio Cicconetti https://ccicconetti.github.io/

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

#include "Support/mmtable.h"

#include "gtest/gtest.h"

namespace uiiit {
namespace support {

struct TestMmTable : public ::testing::Test {};

TEST_F(TestMmTable, test_singlecol) {
  MmTable myTable(1);
  ASSERT_EQ("", myTable.toString());

  myTable(3, 0, "foo");
  ASSERT_EQ("|  |\n| - |\n|  |\n|  |\n| foo |\n", myTable.toString());
}

TEST_F(TestMmTable, test_headeronly) {
  MmTable myTable(2);
  myTable(0, 0, "foo");
  myTable(0, 1, "bar");
  ASSERT_EQ("| foo | bar |\n| - | - |\n| | |\n", myTable.toString());
}

TEST_F(TestMmTable, test_multicol) {
  MmTable myTable(3);

  for (auto i = 0; i < 3; i++) {
    for (auto j = 0; j < 3; j++) {
      myTable(i, j, std::to_string(i) + "," + std::to_string(j));
    }
  }
  ASSERT_EQ("| 0,0 | 0,1 | 0,2 |\n"
            "| - | - | - |\n"
            "| 1,0 | 1,1 | 1,2 |\n"
            "| 2,0 | 2,1 | 2,2 |\n",
            myTable.toString());
}

TEST_F(TestMmTable, test_outofrange) {
  ASSERT_THROW(MmTable(0), std::runtime_error);

  MmTable myTable(3);
  ASSERT_NO_THROW(myTable(0, 0, "foo"));
  ASSERT_NO_THROW(myTable(0, 1, "foo"));
  ASSERT_NO_THROW(myTable(0, 2, "foo"));
  ASSERT_THROW(myTable(0, 3, "foo"), std::runtime_error);
  ASSERT_THROW(myTable(0, 4, "foo"), std::runtime_error);
}

} // namespace support
} // namespace uiiit
