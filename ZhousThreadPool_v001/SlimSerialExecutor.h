#pragma once
//
#include "ThreadWapper.h"
//
#include <chrono>
//
#include <queue>
#include <unordered_set>
//
#include <future>
#include <mutex>
//
namespace CustomerDefined {
class SlimSerialExecutor {
public:
  using TaskRunnerTag_Ty = uint64_t;
  using RepeatedTaskId_Ty = uint64_t;
  // 执行线程
  using TaskRunner_Ty = ThreadWapper;
  using TaskRunnerUPtr_Ty = std::unique_ptr<TaskRunner_Ty>;
  //
  using Clock_Ty = std::chrono::high_resolution_clock;
  using TimePoint_Ty = std::chrono::time_point<Clock_Ty>;
  using Duration_Ty = std::chrono::microseconds;
  //
  using TaskItem_Ty = std::function<void()>;
  //
  static constexpr const uint64_t InfiniteRepeated{-1};
  static constexpr const uint64_t InvalidRepeatedTaskId{-1};

private:
  // PriorityTimerImpl 前向声明
  // 内部实现
  class PriorityTimerImpl;
  /* PriorityTimer
   * 维护内部线程和任务队列（以发生时间点为权的优先级队列）
   */
  class PriorityTimer {
  private:
    std::unique_ptr<PriorityTimerImpl> PTimerUPtr;

  public:
    PriorityTimer();
    //
    PriorityTimer(const PriorityTimer &) = delete;
    //
    ~PriorityTimer();
    //
    PriorityTimer &operator=(const PriorityTimer &) = delete;
    /* Start
     * 如果RunningTag为true，则无操作
     * 如果RunningTag为false，设置RunningTag
     * 会清理内部容器
     */
    void Start();
    /* Stop
     * 设置RunningTag为false
     * 清空内部容器
     */
    void Stop();
    /* Resume
     * 此接口 不 会清理内部容器
     * 如果暂停期间堆积的任务过多需要自行判断是否清理队列
     */
    void Resume();
    /* Pause
     * 挂起线程 一段时间
     * 指定时间其实是添加一项实时任务
     */
    void Pause();
    void Pause(const std::chrono::microseconds &timeoffMicroSec);
    /* Add系列
     * 添加常规（无参数）任务
     * 如果不指定周期和重复次数默认重复1次（接口1）
     * 不会返回RepeatedTaskId，所以不会记录为重复任务，也不能取消
     * （接口2）可以根据需要管理RepeatedTaskId
     * 可以尝试取消该重复任务，但已经添加进队列中的任务不会取消
     * 为repCount指定 InfiniteRepeated 周期性重复直到取消为止
     */
    void AddRealTimeTask(TaskItem_Ty &&titem);
    void AddNormalTask(TaskItem_Ty &&titem, Duration_Ty delayMicroSec);
    //
    [[nodiscard]] RepeatedTaskId_Ty AddNormalTask(
        TaskItem_Ty &&titem, const std::chrono::microseconds &delayMicroSec,
        const std::chrono::microseconds &periodMicroSec, uint64_t repCount);
    /* TryCancelSpecifiedTask
     * 尝试删除一个已添加的NormalTask,
     * rtaskId通过AddNormalTask获得
     * 已进入队列的任务无法取消
     */
    bool TryCancelSpecifiedTask(RepeatedTaskId_Ty rtaskId);
    /* DeleteAllTasts
     * 清空内部容器
     */
    void DeleteAllTasks();
    /* Post系列
     * 只提供需要参数有返回值任务的转发
     * 不提供重复任务(封装太麻烦)
     * 不用返回值时由调用者封装
     */
    template <typename Func_Ty, typename... Args_Ty>
    auto PostRealTimeTask(Func_Ty &&func, Args_Ty... args)
        -> std::future<std::result_of_t<Func_Ty(Args_Ty...)>> {
      using Result_Ty = std::result_of_t<Func_Ty(Args_Ty...)>;
      auto titemSPtr = std::make_shared<std::packaged_task<Result_Ty>>(
          std::bind(std::forward<Func_Ty>(std::move(func)),
                    std::forward<Args_Ty>(args)...));
      Result_Ty resFu = titemSPtr->get_future();
      AddRealTimeTask([titemSPtr]() { (*titemSPtr)(); })
    }
    //
    template <typename Du_Ty, typename Func_Ty, typename... Args_Ty>
    auto PostNormalTask(Du_Ty delay, Func_Ty &&func, Args_Ty &&...args)
        -> std::future<std::result_of_t<Func_Ty(Args_Ty...)>> {
      using Result_Ty = std::result_of_t<Func_Ty(Args_Ty...)>;
      auto titemSPtr = std::make_shared<std::packaged_task<Result_Ty>>(
          std::bind(std::forward<Func_Ty>(std::move(func)),
                    std::forward<Args_Ty>(args)...));
      Result_Ty resFu = titemSPtr->get_future();
      AddRealTimeTask([titemSPtr, delay]() { (*titemSPtr)(); },
                      std::chrono::duration_cast<Duration_Ty>(delay));
    }
  }; // class PriorityTimer
private:
  PriorityTimer PTimer;
}; // class SlimSerialExecutor
} // namespace CustomerDefined