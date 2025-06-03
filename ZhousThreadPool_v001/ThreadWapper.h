#pragma once
#include <functional>
#include <thread>

/* struct ThreadWapper
 * Thread包装器
 * 以头文件的形式提供
 */
struct ThreadWapper {
  ThreadWapper() = delete;
  ThreadWapper(const ThreadWapper &) = delete;
  // ThreadWapper(ThreadWapper&&)=default;
  //
  explicit ThreadWapper(std::function<void(void)> tfunc)
      : InternalThread{std::move(tfunc)} {}
  //
  ~ThreadWapper() { Join(); }
  ThreadWapper &operator=(const ThreadWapper &) = delete;
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
}; // struct ThreadWapper