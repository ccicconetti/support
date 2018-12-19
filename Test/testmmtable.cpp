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
