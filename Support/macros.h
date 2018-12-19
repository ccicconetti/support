#pragma once

#define NONCOPYABLE_NONMOVABLE(X)                                              \
  X(const X&) = delete;                                                        \
  X& operator=(const X&) = delete;

#define MOVEONLY(X)                                                            \
  X(X&&)     = default;                                                        \
  X& operator=(X&&) = default;                                                 \
  X(const X&)       = delete;                                                  \
  X& operator=(const X&) = delete;

#ifndef NDEBUG
#define ASSERT_IS_LOCKED(X)                                                    \
  {                                                                            \
    std::unique_lock<std::mutex> lck(X, std::try_to_lock);                     \
    assert(not lck);                                                           \
  }
#else
#define ASSERT_IS_LOCKED(X)
#endif

#ifndef NDEBUG
#define ASSERT_IS_NOT_LOCKED(X)                                                \
  {                                                                            \
    std::unique_lock<std::mutex> lck(X, std::try_to_lock);                     \
    assert(lck);                                                               \
  }
#else
#define ASSERT_IS_NOT_LOCKED(X)
#endif

// from gtest_prod.h
#define FRIEND_TEST(test_case_name, test_name)                                 \
  friend class test_case_name##_##test_name##_Test
