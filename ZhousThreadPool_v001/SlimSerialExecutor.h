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
  static constexpr const unsigned int InfiniteRepeated{-1};

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
    std::priority_queue<PriorityTaskItem> TaskItemPQueue;
    //
    std::unordered_set<RepeatedTaskId_Ty> RepeatedTaskIdUSet;
    //
    std::unique_ptr<ThreadWapper> InternalThreadUPtr;
    //
    std::mutex InternalMutex;
    std::condition_variable CondVar;
    //
    std::atomic<bool> RunningTag;
    //
    std::atomic<RepeatedTaskId_Ty> NextRepeatedTaskId;

  public:
    PriorityTimer();
    //
    PriorityTimer(const PriorityTimer &) = delete;
    //
    ~PriorityTimer();
    //
    PriorityTimer &operator=(const PriorityTimer &) = delete;
    /* Run
     * 如果RunningTag为true，则无操作
     * 如果RunningTag为false，设置RunningTag
     * 会清理内部容器
     */
    void Start();
    /* ReStart;
     * 如果RunningTag为true，则无操作
     * 如果RunningTag为false，设置RunningTag
     * 不 会清理内部容器
     * 如果暂停期间堆积的任务过多需要自行判断是否清理队列
     */
    void ReStart();
    /* Pause
     * 挂起线程 一段时间
     * 不 会清理内部容器
     * 如果暂停期间堆积的任务过多需要自行判断是否清理队列
     */
    void Pause();
    void Pause(const std::chrono::microseconds&timeoffMicroSec);
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
     * InfiniteRepeated重复直到取消为止
     */
    RepeatedTaskId_Ty AddNormalTask(
        TaskItem_Ty titem, const std::chrono::microseconds &delayedMicroSec,
        const std::chrono::microseconds &periodMicroSec, unsigned int repCount);
    RepeatedTaskId_Ty
    AddNormalTask(TaskItem_Ty titem,
                  const std::chrono::microseconds &delayedMicroSec);
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
    void Run_();
    //
    void DefThreadFunc();

  }; // class PriorityTimer

}; // class SlimSerialExecutor
} // namespace CustomerDefined