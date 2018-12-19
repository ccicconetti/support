#pragma once

#include <memory>

namespace uiiit {
namespace support {
namespace detail {

template <class T>
struct Caller {
  void call(T& t) {
    t();
  }
  void stop(T& t) {
    t.stop();
  }
};

template <class T>
struct Caller<T*> {
  void call(T* t) {
    assert(t != nullptr);
    (*t)();
  }
  void stop(T* t) {
    assert(t != nullptr);
    t->stop();
  }
};

template <class T>
struct Caller<std::unique_ptr<T>> {
  void call(std::unique_ptr<T>& t) {
    assert(t);
    (*t)();
  }
  void stop(std::unique_ptr<T>& t) {
    assert(t);
    t->stop();
  }
};

} // namespace detail
} // namespace support
} // namespace uiiit
