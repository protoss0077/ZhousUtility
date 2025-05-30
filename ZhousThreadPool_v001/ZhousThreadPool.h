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
#include <assert.h>
//
namespace CustomerDefined {
// 前向声明
// thread的包装器
// 析构会调用join
struct ThreadWapper;
using ThreadWapperSPtr_Ty = std::shared_ptr<ThreadWapper>;
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
  // 空闲时从池中移除线程的超时数(分钟)
  static constexpr const unsigned int AutoRemoveTimeOut{1};

private:
  std::vector<ThreadWapperSPtr_Ty> ThreadSPtrColl;
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
  // std::atomic<bool> InitTag;
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
  /* Start
   * 设置RunningTag，添加第1个线程
   * 如果RunningTag已设置不会执行操作
   */
  void Start();
  //
  bool IsRunning() { return RunningTag.load(); }
  //
  void Stop();
  /* TryAddThread
   * 满足条件时为线程池添加一个工作线程
   * 条件：
   * InPoolThreadCount小于MaxThreadCountLimit
   * 且队列不为空
   * 且WorkingThreadCount == InPoolThreadCount
   * 返回InPoolThreadCount的当前值
   */
  unsigned int TryAddThread();
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
    if (!RunningTag.load())
      return nullptr;
    //
    using Result_Ty = std::result_of_t<TFunc_Ty(Args_Ty...)>;
    // std::bind更容易写
    // 此lambda过于复杂，...args的捕获需要C++20?
    /*
    auto taskSPtr = std::make_shared<std::packaged_task<Result_Ty()>>(
        [tf = std::forward<TFunc_Ty>(tfunc),
         ... args = std::forward<Args_Ty>(args)]() mutable -> Result_Ty {
          return std::invoke(tf, std::forward<Args_Ty>(args)...);
        });
        */
    auto taskSPtr = std::make_shared<std::packaged_task<Result_Ty()>>(std::bind(
        std::forward<TFunc_Ty>(tfunc), std::forward<Args_Ty>(args)...));
    std::future<Result_Ty> resFu = taskSPtr->get_future();
    {
      std::lock_guard<std::mutex> lk(InternalMutex);
      TaskItemQueue.emplace([taskSPtr]() { (*taskSPtr)(); });
    }
    CondVar.notify_all();
    return std::make_shared<std::future<Result_Ty>>(std::move(resFu));
  }

private:
  /* ConfirmThreadCount
   * 确定最终的ThreadCountLimit
   */
  static unsigned int ConfirmThreadCount(unsigned int expectCount);
  /* DefThreadFunc
   * 线程函数
   */
  void DefThreadFunc();

}; // class ZhousThreadPool
/*
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
    if (Joinable())
      InternalThread.join();
  }
  //
  std::thread::id GetId() { return InternalThread.get_id(); }

private:
  std::thread InternalThread;

}; // struct ThreadWapper

} // namespace CustomerDefined