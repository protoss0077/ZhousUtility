#pragma once
//
#include "ThreadWrapper.h"
//
#include <chrono>
//
#include <queue>
#include <unordered_set>
//
#include <future>
#include <mutex>
//
#ifdef _DEBUG
#include <iostream>
#endif // _DEBUG

//
namespace CustomerDefined {
/************************************
 * class SlimSerialExecutor *
 * 以时间序管理工作项，顺序执行的单线程包装器 *
 ************************************
 */
class SlimSerialExecutor {
public:
  using TaskRunnerTag_Ty = uint64_t;
  using RepeatedTaskId_Ty = uint64_t;
  // 执行线程
  using TaskRunner_Ty = ThreadWrapper;
  using TaskRunnerUPtr_Ty = std::unique_ptr<TaskRunner_Ty>;
  //
  using Clock_Ty = std::chrono::high_resolution_clock;
  using TimePoint_Ty = std::chrono::time_point<Clock_Ty>;
  using Duration_Ty = std::chrono::microseconds;
  //
  using TaskItem_Ty = std::function<void()>;
  //
  static constexpr const uint64_t InfiniteRepeated{static_cast<uint64_t>(-1)};
  static constexpr const uint64_t InvalidRepeatedTaskId{
      static_cast<uint64_t>(-1)};
  //
  /* enum class State
   * 标记Timer内部状态
   */
  enum class RunnerState { Stopped, Running, Paused }; // enum class State
  static std::string GetRunnerStateString(const RunnerState &tar) {
    switch (tar) {
    case RunnerState::Running:
      return "RunnerState::Running";
    case RunnerState::Stopped:
      return "RunnerState::Stopped";
    case RunnerState::Paused:
      return "RunnerState::Paused";
    default:
      return "Unknow State";
    } // switch
  }
  //
  using State_Ty = RunnerState;

private:
  // PriorityTimerImpl 前向声明
  // 内部实现
  class PriorityTimerImpl;
  /* PriorityTimer
   * 维护内部线程和任务队列（以发生时间点为权的优先级队列）
   */

private:
  std::unique_ptr<PriorityTimerImpl> PTimerUPtr;

public:
  SlimSerialExecutor();
  //
  SlimSerialExecutor(const SlimSerialExecutor &) = delete;
  //
  ~SlimSerialExecutor();
  //
  SlimSerialExecutor &operator=(const SlimSerialExecutor &) = delete;
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
  /* IsRunning
   * 是否运行的快照
   */
  bool IsRunning() const;
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
  //
  State_Ty GetState() const;
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
#if _MSVC_LANG && (__cplusplus >= 202002L || _MSVC_LANG >= 202002L)
  // C++20+
  // !!!AI建议，仔细检查
  template <typename Func_Ty, typename... Args_Ty>
  auto PostRealTimeTask(Func_Ty &&func, Args_Ty &&...args)
      -> std::future<std::invoke_result_t<Func_Ty, Args_Ty...>> {
    if (!IsRunning()) {
      throw std::runtime_error("SlimSerialExecutor未运行...");
    }

    using Result_Ty = std::invoke_result_t<Func_Ty, Args_Ty...>;
    auto task = std::packaged_task<Result_Ty()>(
        [func = std::forward<Func_Ty>(func),
         ... args = std::forward<Args_Ty>(args)]() mutable {
          return std::invoke(std::move(func), std::move(args)...);
        });

    auto res_fu = task.get_future();
    AddRealTimeTask([task = std::move(task)]() mutable { task(); });

    return res_fu;
  }
#else // C++20-
#if _MSVC_LANG && (_MSVC_LANG >= 201703L || __cplusplus >= 201703L)
  // C++17+
  // !!!AI建议，仔细检查
  template <typename Func_Ty, typename... Args_Ty>
  auto PostRealTimeTask(Func_Ty &&func, Args_Ty &&...args) -> std::future<
      std::invoke_result_t<std::decay_t<Func_Ty>, std::decay_t<Args_Ty>...>> {
    if (!IsRunning()) {
      throw std::runtime_error("SlimSerialExecutor未运行...");
    }

    using Result_Ty =
        std::invoke_result_t<std::decay_t<Func_Ty>, std::decay_t<Args_Ty>...>;

    auto task_ptr = std::make_shared<std::packaged_task<Result_Ty()>>(
        [func = std::forward<Func_Ty>(func),
         ... args = std::forward<Args_Ty>(args)]() mutable {
          return std::invoke(std::move(func), std::move(args)...);
        });

    auto res_fu = task_ptr->get_future();
    AddRealTimeTask(
        [task_ptr = std::move(task_ptr)]() mutable { (*task_ptr)(); });

    return res_fu;
  }
#else // C++17-
  template <typename Func_Ty, typename... Args_Ty>
  [[nodiscard]] auto PostRealTimeTask(Func_Ty &&func, Args_Ty... args)
      -> std::future<std::result_of_t<Func_Ty(Args_Ty...)>> {
    if (!IsRunning())
      throw std::runtime_error("SlimSerialExecutor未运行...");
    using Result_Ty = std::result_of_t<Func_Ty(Args_Ty...)>;
    auto titemSPtr = std::make_shared<std::packaged_task<Result_Ty()>>(
        std::bind(std::forward<Func_Ty>(std::move(func)),
                  std::forward<Args_Ty>(args)...));
    std::future<Result_Ty> resFu = titemSPtr->get_future();
    AddRealTimeTask([titemSPtr]() { (*titemSPtr)(); });
    return resFu;
  }
  //
  template <typename Du_Ty, typename Func_Ty, typename... Args_Ty>
  [[nodiscard]] auto PostNormalTask(Du_Ty delay, Func_Ty &&func,
                                    Args_Ty &&...args)
      -> std::future<std::result_of_t<Func_Ty(Args_Ty...)>> {
    if (!IsRunning())
      throw std::runtime_error("SlimSerialExecutor未运行...");
    using Result_Ty = std::result_of_t<Func_Ty(Args_Ty...)>;
    auto titemSPtr = std::make_shared<std::packaged_task<Result_Ty()>>(
        std::bind(std::forward<Func_Ty>(std::move(func)),
                  std::forward<Args_Ty>(args)...));
    std::future<Result_Ty> resFu = titemSPtr->get_future();
    AddNormalTask([titemSPtr, delay]() { (*titemSPtr)(); },
                  std::chrono::duration_cast<Duration_Ty>(delay));
    return resFu;
  }
#endif
#endif //

}; // class SlimSerialExecutor
/************************************
 * class SlimExecutor *
 * 异步执行任务的单线程包装器 *
 ************************************
 */
class SlimExecutor {
public:
  // 执行线程
  using TaskRunner_Ty = ThreadWrapper;
  using TaskRunnerUPtr_Ty = std::unique_ptr<TaskRunner_Ty>;
  //
  using TaskItem_Ty = std::function<void()>;
  //
  using State_Ty = SlimSerialExecutor::RunnerState;

private:
  // 内部实现
  class SlimExecutorImpl;

private:
  std::unique_ptr<SlimExecutorImpl> ExecutorUPtr;

public:
  SlimExecutor();
  SlimExecutor(const SlimExecutor &) = delete;
  //
  ~SlimExecutor();
  //
  SlimExecutor &operator=(const SlimExecutor &) = delete;
  // 公共接口
  void Start();
  //
  void Stop();
  //
  bool IsRunning() const;
  //
  void Resume();
  //
  void Pause();
  //
  State_Ty GetState() const;
  //
  void AddTask(TaskItem_Ty &&titem);
  //
  template <typename Func_Ty, typename... Args_Ty>
  void PostNoReturnTask(Func_Ty &&func, Args_Ty &&...args) {
    if (!IsRunning())
      throw std::runtime_error("SlimExecutor未运行...");
    auto titem =
        std::bind(std::forward<Func_Ty>(func), std::forward<Args_Ty>(args)...);
    AddTask(std::move([titem]() { titem(); }));
  }
  //
  template <typename Func_Ty, typename... Args_Ty>
  [[nodiscard]] auto PostTask(Func_Ty &&func, Args_Ty &&...args)
      -> std::future<std::result_of_t<Func_Ty(Args_Ty...)>> {
    if (!IsRunning())
      throw std::runtime_error("SlimExecutor未运行...");
    using Result_Ty = std::result_of_t<Func_Ty(Args_Ty...)>;
    auto taskItemSPtr = std::make_shared<std::packaged_task<Result_Ty()>>(
        std::bind(std::forward<Func_Ty>(func), std::forward<Args_Ty>(args)...));
    std::future<Result_Ty> resFu = taskItemSPtr->get_future();
    AddTask(std::move([taskItemSPtr]() { (*taskItemSPtr)(); }));
    return resFu;
  }
}; // class SlimExecutor

} // namespace CustomerDefined