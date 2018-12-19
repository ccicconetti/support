#include "Support/movingwnd.h"
#include "macros.h"

#include "gtest/gtest.h"

#include <chrono>
#include <thread>

namespace uiiit {
namespace support {

struct TestMovingWnd : public ::testing::Test {};

TEST_F(TestMovingWnd, test_ctor) {
  ASSERT_NO_THROW(MovingWnd<int>(1));
}

TEST_F(TestMovingWnd, test_exceptions) {
  ASSERT_THROW(MovingWnd<int>(0), ZeroWindow);
}

TEST_F(TestMovingWnd, test_int) {
  MovingWnd<int> myWnd(5);

  ASSERT_EQ(std::vector<int>({}), myWnd.values());
  ASSERT_TRUE(myWnd.empty());
  ASSERT_FALSE(myWnd.full());
  ASSERT_EQ(0u, myWnd.size());

  myWnd.add(7);
  ASSERT_EQ(std::vector<int>({7}), myWnd.values());
  ASSERT_FALSE(myWnd.empty());
  ASSERT_EQ(1u, myWnd.size());

  for (int i = 0; i < 5; i++) {
    myWnd.add(i);
  }
  ASSERT_EQ(std::vector<int>({0, 1, 2, 3, 4}), myWnd.values());
  ASSERT_FALSE(myWnd.empty());
  ASSERT_TRUE(myWnd.full());
  ASSERT_EQ(5u, myWnd.size());

  myWnd.add(7);
  ASSERT_EQ(std::vector<int>({1, 2, 3, 4, 7}), myWnd.values());
  ASSERT_TRUE(myWnd.full());
  ASSERT_FALSE(myWnd.empty());
  ASSERT_EQ(5u, myWnd.size());

  myWnd.clear();
  ASSERT_EQ(std::vector<int>({}), myWnd.values());
  ASSERT_TRUE(myWnd.empty());
  ASSERT_FALSE(myWnd.full());
  ASSERT_EQ(0u, myWnd.size());

  myWnd.add(-1);
  myWnd.add(0);
  myWnd.add(1);
  ASSERT_EQ(std::vector<int>({-1, 0, 1}), myWnd.values());
  ASSERT_FALSE(myWnd.empty());
  ASSERT_EQ(3u, myWnd.size());

  ASSERT_EQ(-1, myWnd.pop());
  ASSERT_EQ(0, myWnd.pop());
  ASSERT_EQ(1, myWnd.pop());
  ASSERT_THROW(myWnd.pop(), EmptyWindow);
}

TEST_F(TestMovingWnd, test_purge) {
  MovingWnd<int> myWnd(5);
  for (int i = 0; i < 4; i++) {
    myWnd.add(i);
  }
  ASSERT_EQ(std::vector<int>({0, 1, 2, 3}), myWnd.values());
  ASSERT_FALSE(myWnd.purge(1));
  ASSERT_EQ(std::vector<int>({0, 1, 2, 3}), myWnd.values());

  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  myWnd.add(4);
  ASSERT_EQ(std::vector<int>({0, 1, 2, 3, 4}), myWnd.values());

  WAIT_FOR(
      [&myWnd]() {
        myWnd.purge(0.1);
        return myWnd.values().size() == 1;
      },
      0.15)
      << myWnd.values().size();
  ASSERT_EQ(std::vector<int>({4}), myWnd.values());

  WAIT_FOR([&myWnd]() { return myWnd.purge(0.1); }, 0.15);
  ASSERT_EQ(std::vector<int>({}), myWnd.values());
  ASSERT_TRUE(myWnd.empty());
}

} // namespace support
} // namespace uiiit
