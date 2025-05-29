#pragma once
//
#include <functional>
//
#include <queue>
#include <vector>
//
#include <future>
#include <mutex>
//
namespace CustomerDefined {
// 前向声明
// std::shared_ptr<std::thread>的包装器
// 重写了析构
struct ThreadSPtrWapper;
// 队列内工作项
using TaskItem_Ty = std::function<void(void)>;
/* class ZhousThreadPool
 */
class ZhousThreadPool {
public:
  // 最大线程数限制
  static const unsigned int MaxThreadCountLimit;
  // 最小线程数限制
  static const unsigned int MinThreadCountLimit;

private:
  std::vector<ThreadSPtrWapper> ThreadSPtrColl;
  std::queue<TaskItem_Ty> TaskItemQueue;
  //
  std::mutex InternalMutex;
  std::condition_variable CondVar;
  /* 设置最大/最小数量限制
   * 监视当前数量及工作线程数量
   */
  std::atomic<unsigned int> WorkingThreadCount;
  std::atomic<unsigned int> InPoolThreadCount;
  // 此值由最大/最小/用户输入值确定，只读
  const unsigned int ThreadCountLimit;
  // 初始化标记 true/false 已初始化/未初始化
  std::atomic<bool> InitTag;
  // 运行标记 true/false
  std::atomic<bool> RunningTag;

public:
  //
  explicit ZhousThreadPool(unsigned int expectTCount);
  //
  ZhousThreadPool(const ZhousThreadPool &) = delete;
  //
  ~ZhousThreadPool();
  //
  ZhousThreadPool &operator=(const ZhousThreadPool &) = delete;
  //
  bool Start();
  //
  void Stop();
  /* AddThread
   * 满足条件时为线程池添加一个工作线程
   * 条件：
   * InPoolThreadCount小于MaxThreadCountLimit
   * 且队列不为空
   * 且WorkingThreadCount == InPoolThreadCount
   * 返回InPoolThreadCount的当前值
   */
  unsigned int AddThread();
  /* Remove
   * 满足条件时移除一个工作线程
   * 条件:
   * InPoolThreadCount大于MinThreadCountLimit
   * 且队列为空,此时不应有WorkingThread(==0)
   * 返回InPoolThreadCount的当前值
   */
  unsigned int Remove();
  /*
   */
  template <typename TFunc_Ty, typename... Args_Ty>
  [[nodiscard]] auto AddTaskItem(TFunc_Ty &&tfunc, Args_Ty &&...args)
      -> std::shared_ptr<std::future<std::result_of_t<TFunc_Ty(Args_Ty...)>>> {
    if (!InitTag.load() || !RunningTag())
      return nullptr;
    //
    using Result_Ty = std::result_of_t<TFunc_Ty(Args_Ty...)>;
    auto taskSPtr = std::make_shared<std::packaged_task<Result_Ty()>>(
      // 可能std::bind更容易写
        [tf = std::forward<TFunc_Ty>(tfunc),
         ... args = std::forward<Args_Ty>(args)]() mutable -> Result_Ty {
          return std::invoke(tf, std::forward<Args_Ty>(args)...);
        });
    std::future<Result_Ty> resFu = taskSPtr->get_future();
    {
      std::lock_guard<std::mutex> lk(InternalMutex);
      TaskItemQueue.emplace([taskSPtr]() { (*taskSPtr)(); });
    }
    CondVar.notify_all();
    return std::make_shared<std::future<Result_Ty>>(std::move(resFu));
  }

  private:
  unsigned int ConfirmThreadCount(unsigned int expectCount);
}; // class ZhousThreadPool
/*
 */
struct ThreadSPtrWapper {
  ThreadSPtrWapper() = default;
  ThreadSPtrWapper(const ThreadSPtrWapper &) = delete;
  ~ThreadSPtrWapper();
  ThreadSPtrWapper &operator=(const ThreadSPtrWapper &) = delete;

private:
  std::shared_ptr<std::thread> ThreadSPtr{nullptr};

}; // struct ThreadSPtrWapper

} // namespace CustomerDefined