#pragma once
//
#include <string>
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
  // 空闲时从池中移除线程的超时数(milliseconds)
  static constexpr const unsigned int AutoRemoveTimeOut{1000};

private:
  // 移除标记的可用值
  static constexpr const unsigned int RemoveStateNone{0};
  static constexpr const unsigned int RemoveStateLastIdle{1};
  static constexpr const unsigned int RemoveStateLastEnded{2};

public:
  /* struct PoolSnapshot
   * 信息快照
   */
  struct PoolSnapshot {
    bool RunningTag;
    unsigned int WorkingThreadCount;
    unsigned int InPoolThreadCount;
    unsigned int ThreadCountLimit;
    size_t TaskItemInQueue;
    //
    PoolSnapshot() = delete;
    PoolSnapshot(const PoolSnapshot &) = default;
    PoolSnapshot(bool rtag, unsigned int wtc, unsigned int itc,
                 unsigned int tcl, size_t tiiq);
    //
    ~PoolSnapshot() = default;
    //
    PoolSnapshot &operator=(const PoolSnapshot &) = default;
    /* ToString
     * tabViewTag为true,ThreadCountLimit<=9时返回列表视图字符串
     * 比如:
     * Running:|0 * !1 _ 2 x 3 x 4|| 3 |
     * 表示线程池在运行
     * 池内最小线程数为1，最大线程数为4，当前池内线程数为2，工作线程数为1，闲置线程数为1
     * 工作项队列内不为空，有3项等待处理
     */
    std::string ToString(bool tabViewTag) const;
    //
  private:
    std::string ToTableString() const;
    //
    std::string ToDefString() const;
  };

private:
  // 也许可以通过一个定容数组来代替？
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
  // 运行标记 true/false
  std::atomic<bool> RunningTag;
  // 移除标记
  std::atomic<unsigned int> RemoveState;

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
   * 设置RunningTag，添加0号线程
   * 如果RunningTag已设置不会执行操作
   */
  void Start();
  //
  bool IsRunning() const { return RunningTag.load(); }
  //
  void Stop();
  /* TryAddThread
   * 满足条件时尝试为线程池添加一个工作线程
   * 条件：
   * InPoolThreadCount小于MaxThreadCountLimit
   * 且队列不为空且没有闲置线程
   * 返回InPoolThreadCount的当前值
   */
  unsigned int TryAddThread();
  /* GetSnapshot
   * 获取快照
   */
  PoolSnapshot GetSnapshot() const;
  /*
   */
  template <typename TFunc_Ty, typename... Args_Ty>
  void AddNoReturnTaskItem(TFunc_Ty &&tfunc, Args_Ty &&...args) {
    if (!RunningTag.load())
      throw std::runtime_error("线程池未运行...");
    auto titem = std::bind(std::forward<TFunc_Ty>(tfunc),
                           std::forward<Args_Ty>(args)...);
    {
      std::lock_guard<std::mutex> lk(InternalMutex);
      TaskItemQueue.emplace([titem]() { titem(); });
    }
    CondVar.notify_all();
  }
  /* AddTaskItem
   * 添加工作项
   * 去了掉shared_ptr的嵌套
   */
  template <typename TFunc_Ty, typename... Args_Ty>
  [[nodiscard]] auto AddTaskItem(TFunc_Ty &&tfunc, Args_Ty &&...args)
      -> std::future<std::result_of_t<TFunc_Ty(Args_Ty...)>> {
    if (!RunningTag.load())
      throw std::runtime_error("线程池未运行...");
    //
    using Result_Ty = std::result_of_t<TFunc_Ty(Args_Ty...)>;
    // std::bind更容易写
    // 此lambda过于复杂，...args的捕获需要C++20?
    /*
    auto taskItem = std::make_shared<std::packaged_task<Result_Ty()>>(
        [tf = std::forward<TFunc_Ty>(tfunc),
         ... args = std::forward<Args_Ty>(args)]() mutable -> Result_Ty {
          return std::invoke(tf, std::forward<Args_Ty>(args)...);
        });
    */
    // 这里必须用指针！！！
    auto taskItemSPtr =
        std::make_shared<std::packaged_task<Result_Ty()>>(std::bind(
            std::forward<TFunc_Ty>(tfunc), std::forward<Args_Ty>(args)...));
    std::future<Result_Ty> resFu = taskItemSPtr->get_future();
    {
      std::lock_guard<std::mutex> lk(InternalMutex);
      TaskItemQueue.emplace([taskItemSPtr]() { (*taskItemSPtr)(); });
    }
    CondVar.notify_all();
    return resFu;
  }

private:
  /* ConfirmThreadCount
   * 确定最终的ThreadCountLimit
   */
  static unsigned int ConfirmThreadCount(unsigned int expectCount);
  /* DefThreadFunc
   * 其他线程函数
   * 其他线程由Thread0添加/移出
   * 它们只是接任务做任务
   */
  void DefThreadFunc();
  /* ThreadFunc0
   * 0号线程函数
   */
  void Thread0Func();

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
    if (InternalThread.joinable())
      InternalThread.join();
  }
  //
  std::thread::id GetId() { return InternalThread.get_id(); }

private:
  std::thread InternalThread;
}; // struct ThreadWapper

} // namespace CustomerDefined