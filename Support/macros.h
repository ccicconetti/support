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
