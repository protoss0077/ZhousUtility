#pragma once
//
#include "ThreadWapper.h"
//
#include <chrono>
//
#include <queue>
#include <unordered_set>
//
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
  using TaskItem_Ty = std::function<void()>;
  //
  static constexpr const uint64_t InfiniteRepeated{-1};
  static constexpr const uint64_t InvalidRepeatedTaskId{-1};

private:
  /*struct PriorityTaskItem
   * 优先级队列中维护的项
   * 以时间点为权与任务函数关联
   * 内部使用
   */
  struct PriorityTaskItem {
    using OriginTaskItem_Ty = TaskItem_Ty;
    //
    TimePoint_Ty StartTp;
    //
    OriginTaskItem_Ty TaskItem;
    //
    RepeatedTaskId_Ty RepeatedId;
    //
    PriorityTaskItem() = delete;
    //
    PriorityTaskItem(const PriorityTaskItem &) = default;
    //
    PriorityTaskItem(const TimePoint_Ty stp, OriginTaskItem_Ty &&otitem,
                     const RepeatedTaskId_Ty rtId = InvalidRepeatedTaskId)
        : StartTp{stp}, TaskItem{std::move(otitem)}, RepeatedId{rtId} {}
    //
    ~PriorityTaskItem() {}
    //
    PriorityTaskItem &operator=(const PriorityTaskItem &) = default;
    //
    bool operator<(const PriorityTaskItem &other) const {
      return StartTp > other.StartTp;
    }
  }; // struct PriorityTaskItem

  /* PriorityTimer
   * 维护内部线程和任务队列（以发生时间点为权的优先级队列）
   */
  class PriorityTimer {
  private:
    //
    std::priority_queue<PriorityTaskItem> TaskItemPQueue;
    // 存储重复任务标记,添加重复任务的方式：
    // 在队列中添加一个工作项，该工作项的内容是添加一个执行repnum-1的任务
    std::unordered_set<RepeatedTaskId_Ty> RepeatedTaskIdUSet;
    //
    std::unique_ptr<ThreadWapper> InternalThreadUPtr;
    //
    std::mutex InternalMutex;
    std::condition_variable CondVar;
    // true-running;false-stopped
    std::atomic<bool> RunningTag;
    // true-Paused;false-running;
    std::atomic<bool> PausedTag;
    // 重复任务标记计数
    std::atomic<RepeatedTaskId_Ty> RepeatedTaskId;

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
    /* ReStart;
     * RunningTag-true,PausedTag-true
     * 设置PausedTag为false，唤醒休眠的线程
     * 其余情况均无操作
     * 此接口 不 会清理内部容器
     * 如果暂停期间堆积的任务过多需要自行判断是否清理队列
     */
    void ReStart();
    /* Pause
     * 挂起线程 一段时间
     * 指定时间其实是添加一项实时任务
     */
    void Pause();
    void Pause(const std::chrono::microseconds &timeoffMicroSec);
    /* Stop
     * 设置RunningTag为false
     * 清空内部容器
     */
    void Stop();
    /* AddRealTimeTask
     * 添加一个立即（最高优先级）执行的任务进队列
     */
    void AddRealTimeTask(TaskItem_Ty titem);
    /* AddNormalTask
     * 添加常规任务
     * 如果不指定周期和重复次数默认重复1次
     * 不进RepeatedTaskIdUSet
     * InfiniteRepeated重复直到取消为止
     */
    void AddNormalTask(TaskItem_Ty titem,
                       const std::chrono::microseconds &delayedMicroSec);
    [[nodiscard]] uint64_t AddNormalTask(
        TaskItem_Ty titem, const std::chrono::microseconds &delayedMicroSec,
        const std::chrono::microseconds &periodMicroSec, uint64_t repCount);

    /* TryCancelRepeatedTask
     * 尝试删除一个任务
     * 已开始的任务无法取消
     */
    bool TryCancelSpecifiedTask(RepeatedTaskId_Ty rtaskId);
    /* DeleteAllTasts
     * 清空内部容器
     */
    void DeleteAllTasts();

  private:
    void DefThreadFunc();
    //
    void StayAWhile(const std::chrono::microseconds &timeoffMicroSec);
    //
    RepeatedTaskId_Ty GetNextRepeatedTaskId();
    //
    void InternalAddNormalTask(TaskItem_Ty &&titem, RepeatedTaskId_Ty rtid,
                               const std::chrono::microseconds &delayedMicroSec,
                               const std::chrono::microseconds &periodMicroSec,
                               uint64_t repCount);
    // 此接口用于在添加重复工作项的转发任务到期时
    void TaskExecuteAndAddNext(TaskItem_Ty &&titem, RepeatedTaskId_Ty rtid,
                               const std::chrono::microseconds &periodMicroSec,
                               uint64_t repCount);

  }; // class PriorityTimer

}; // class SlimSerialExecutor
} // namespace CustomerDefined