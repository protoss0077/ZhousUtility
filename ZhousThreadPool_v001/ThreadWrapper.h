#pragma once
#include <functional>
#include <thread>

namespace CustomerDefined {
/* struct ThreadWrapper
 * Thread包装器
 * 以头文件的形式提供
 */
struct ThreadWrapper {
  ThreadWrapper() = delete;
  ThreadWrapper(const ThreadWrapper &) = delete;
  // ThreadWrapper(ThreadWrapper&&)=default;
  //
  explicit ThreadWrapper(std::function<void(void)> tfunc)
      : InternalThread{std::move(tfunc)} {}
  //
  ~ThreadWrapper() { Join(); }
  ThreadWrapper &operator=(const ThreadWrapper &) = delete;
  //
  bool Joinable() { return InternalThread.joinable(); }
  //
  void Join() {
    if (InternalThread.joinable())
      InternalThread.join();
  }
  //
  std::thread::id GetId() { return InternalThread.get_id(); }

private:
  std::thread InternalThread;
}; // struct ThreadWrapper
} // namespace CustomerDefined