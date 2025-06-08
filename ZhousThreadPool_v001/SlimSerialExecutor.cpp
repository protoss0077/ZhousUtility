#include "SlimSerialExecutor.h"

namespace CustomerDefined {
/************************************
 * class SlimSerialExecutor::PriorityTimerImpl 定义 *
 ************************************
 */
class SlimSerialExecutor::PriorityTimerImpl {
public:
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
    //
    void operator()() { TaskItem(); }
  }; // struct PriorityTaskItem
  /* enum class State
   * 标记Timer内部状态
   */
  enum class RunnerState { Stopped, Running, Paused }; // enum class State
  std::string GetRunnerStateString(const RunnerState &tar) {
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
  // 内部成员
  // 任务队列
  std::priority_queue<PriorityTaskItem> TaskItemPQueue;
  // 管理重复任务Id
  std::unordered_set<RepeatedTaskId_Ty> RepeatedIdUSet;
  // 执行线程包装器指针
  TaskRunnerUPtr_Ty TaskRunnerUPtr;
  // 锁
  std::mutex InternalMutex;
  std::condition_variable CondVar;
  //
  std::atomic<RunnerState> State;
  //
  std::atomic<RepeatedTaskId_Ty> ValidRepeatedTaskId;
  //
  PriorityTimerImpl()
      : TaskItemPQueue{}, RepeatedIdUSet{}, TaskRunnerUPtr{nullptr},
        InternalMutex{}, CondVar{}, State{RunnerState::Stopped},
        ValidRepeatedTaskId{0} {}
  //
  PriorityTimerImpl(const PriorityTimerImpl &) = delete;
  //
  ~PriorityTimerImpl() {}
  //
  PriorityTimerImpl &operator=(const PriorityTimerImpl &) = delete;
  //
  void Start();
  //
  void Stop();
  //
  void Pause();
  //
  void Pause(Duration_Ty timeOffMicroSec);
  //
  void Resume();
  //
  void AddRealTimeTask(TaskItem_Ty &&titem);
  //
  void AddNormalTask(TaskItem_Ty &&titem, Duration_Ty delayMicroSec);
  //
  [[nodiscard]] RepeatedTaskId_Ty AddNormalTask(TaskItem_Ty &&titem,
                                                Duration_Ty delayMicroSec,
                                                Duration_Ty periodMicroSec,
                                                uint64_t repCount);
  //
  bool TryCancelTask(RepeatedTaskId_Ty rtid);
  //
  void DeleteAllTask();
  // Utility
  RepeatedTaskId_Ty GetRepeatedId() { return ValidRepeatedTaskId++; }
  // 线程函数
  void ThreadFunc();
  //
  void PostRepeatedTask(TaskItem_Ty &&titem, RepeatedTaskId_Ty rtid,
                        Duration_Ty delayMicroSec, Duration_Ty periodMicroSec,
                        uint64_t repCount);
  //
  void ExecuteAndAddNext(TaskItem_Ty &&titem, RepeatedTaskId_Ty rtid,
                         Duration_Ty periodMicroSec, uint64_t repCount);
}; // class SlimSerialExecutor::PriorityTimerImpl
//
void SlimSerialExecutor::PriorityTimerImpl::Start() {
  std::lock_guard<std::mutex> lk(InternalMutex);
  if (State.load() != RunnerState::Stopped)
    return;
  State.store(RunnerState::Running);
  for (; !TaskItemPQueue.empty();)
    TaskItemPQueue.pop();
  RepeatedIdUSet.clear();
  ValidRepeatedTaskId.store(0);
  TaskRunnerUPtr =
      std::make_unique<TaskRunner_Ty>([this]() { this->ThreadFunc(); });
}
//
void SlimSerialExecutor::PriorityTimerImpl::Stop() {
  {
    std::lock_guard<std::mutex> lk(InternalMutex);
    if (State.load() == RunnerState::Stopped)
      return;
    for (; !TaskItemPQueue.empty();)
      TaskItemPQueue.pop();
    RepeatedIdUSet.clear();
    ValidRepeatedTaskId.store(0);
    State.store(RunnerState::Stopped);
    CondVar.notify_one();
  }
  TaskRunnerUPtr.reset();
}
//
void SlimSerialExecutor::PriorityTimerImpl::Pause() {
  std::lock_guard<std::mutex> lk(InternalMutex);
  if (State.load() != RunnerState::Running)
    return;
  State.store(RunnerState::Paused);
}
//
void SlimSerialExecutor::PriorityTimerImpl::Pause(Duration_Ty timeOffMicroSec) {
  AddRealTimeTask([this, timeOffMicroSec]() {
    {
      std::lock_guard<std::mutex> lk(this->InternalMutex);
      if (State.load() != RunnerState::Running)
        return;
      State.store(RunnerState::Paused);
    }
    std::this_thread::sleep_for(timeOffMicroSec);
    {
      std::lock_guard<std::mutex> lk(this->InternalMutex);
      if (State.load() != RunnerState::Paused)
        return;
      State.store(RunnerState::Paused);
    }
  });
}
//
void SlimSerialExecutor::PriorityTimerImpl::Resume() {
  std::lock_guard<std::mutex> lk(InternalMutex);
  if (State.load() != RunnerState::Paused)
    return;
  State.store(RunnerState::Running);
  CondVar.notify_one();
}
//
void SlimSerialExecutor::PriorityTimerImpl::AddRealTimeTask(
    TaskItem_Ty &&titem) {
  AddNormalTask(std::move(titem), Duration_Ty(0));
}
//
void SlimSerialExecutor::PriorityTimerImpl::AddNormalTask(
    TaskItem_Ty &&titem, Duration_Ty delayMicroSec) {
  auto startTp = Clock_Ty::now() + delayMicroSec;
  bool notifyTag = true;
  {
    std::lock_guard<std::mutex> lk(InternalMutex);
    notifyTag =
        TaskItemPQueue.empty() ? true : startTp <= TaskItemPQueue.top().StartTp;
    TaskItemPQueue.push(std::move(PriorityTaskItem(startTp, std::move(titem))));
  }
  if (notifyTag && State.load() == RunnerState::Running)
    CondVar.notify_one();
}
//
[[nodiscard]] SlimSerialExecutor::RepeatedTaskId_Ty
SlimSerialExecutor::PriorityTimerImpl::AddNormalTask(TaskItem_Ty &&titem,
                                                     Duration_Ty delayMicroSec,
                                                     Duration_Ty periodMicroSec,
                                                     uint64_t repCount) {
  RepeatedTaskId_Ty resId = GetRepeatedId();
  {
    std::lock_guard<std::mutex> lk(InternalMutex);
    RepeatedIdUSet.insert(resId);
  }
  PostRepeatedTask(std::move(titem), resId, delayMicroSec, periodMicroSec,
                   repCount);
  return resId;
}
//
bool SlimSerialExecutor::PriorityTimerImpl::TryCancelTask(
    RepeatedTaskId_Ty rtid) {
  std::lock_guard<std::mutex> lk(InternalMutex);
  if (State.load() == RunnerState::Stopped)
    return false;
  auto fIter = RepeatedIdUSet.find(rtid);
  if (fIter == RepeatedIdUSet.end())
    return false;
  RepeatedIdUSet.erase(rtid);
  return true;
}
//
void SlimSerialExecutor::PriorityTimerImpl::DeleteAllTask() {
  std::lock_guard<std::mutex> lk(InternalMutex);
  for (; !TaskItemPQueue.empty();)
    TaskItemPQueue.pop();
  RepeatedIdUSet.clear();
  ValidRepeatedTaskId.store(0);
}
//
void SlimSerialExecutor::PriorityTimerImpl::ThreadFunc() {
  for (;;) {
    std::unique_lock<std::mutex> ulk(InternalMutex);
    CondVar.wait(ulk, [this]() {
      return State.load() == RunnerState::Stopped ||
             (!TaskItemPQueue.empty() && State == RunnerState::Running);
    });
    if (State.load() == RunnerState::Stopped)
      break;
    if (State.load() == RunnerState::Paused) {
      CondVar.wait(ulk,
                   [this]() { return State.load() != RunnerState::Paused; });
      continue;
    }
    if (TaskItemPQueue.empty())
      continue;
    auto nowTp = Clock_Ty::now();
    auto nextStartTp = TaskItemPQueue.top().StartTp;
    if (nextStartTp > nowTp) {
      CondVar.wait_until(ulk, nextStartTp);
      continue;
    }
    PriorityTaskItem pitem = TaskItemPQueue.top();
    ulk.unlock();
    pitem();
  } // for-loop
}
//
void SlimSerialExecutor::PriorityTimerImpl::PostRepeatedTask(
    TaskItem_Ty &&titem, RepeatedTaskId_Ty rtid, Duration_Ty delayMicroSec,
    Duration_Ty periodMicroSec, uint64_t repCount) {
  auto startTp = Clock_Ty::now() + delayMicroSec;
  bool notifyTag = true;
  {
    std::lock_guard<std::mutex> lk(InternalMutex);
    auto fIter = RepeatedIdUSet.find(rtid);
    if (fIter == RepeatedIdUSet.end())
      return;
    if (repCount == 0) {
      RepeatedIdUSet.erase(fIter);
      return;
    }
    auto postFunc = [this, &titem, rtid, periodMicroSec, repCount]() {
      this->ExecuteAndAddNext(std::move(titem), rtid, periodMicroSec, repCount);
    };
    notifyTag =
        TaskItemPQueue.empty() ? true : startTp <= TaskItemPQueue.top().StartTp;
    TaskItemPQueue.push(
        std::move(PriorityTaskItem(startTp, std::move(postFunc), rtid)));
  } // lk
  if (notifyTag && State.load() == RunnerState::Running)
    CondVar.notify_one();
}
//
void SlimSerialExecutor::PriorityTimerImpl::ExecuteAndAddNext(
    TaskItem_Ty &&titem, RepeatedTaskId_Ty rtid, Duration_Ty periodMicroSec,
    uint64_t repCount) {
  titem();
  auto nextStartTp = Clock_Ty::now() + periodMicroSec;
  bool notifyTag = true;
  uint64_t lastRepCount = (repCount == SlimSerialExecutor::InfiniteRepeated)
                              ? SlimSerialExecutor::InfiniteRepeated
                              : repCount - 1;
  auto postFunc = [this, &titem, rtid, periodMicroSec, lastRepCount]() {
    this->PostRepeatedTask(std::move(titem), rtid, periodMicroSec,
                           periodMicroSec, lastRepCount);
  };
  {
    std::lock_guard<std::mutex> lk(InternalMutex);
    notifyTag = TaskItemPQueue.empty()
                    ? true
                    : nextStartTp <= TaskItemPQueue.top().StartTp;
    TaskItemPQueue.push(
        PriorityTaskItem(nextStartTp, std::move(postFunc), rtid));
  } // lk
  if (notifyTag && State.load() == RunnerState::Running)
    CondVar.notify_one();
}
/************************************
 * SlimSerialExecutor，定义 *
 * 外部接口封装 *
 ************************************
 */
SlimSerialExecutor::PriorityTimer::PriorityTimer() : PTimerUPtr{nullptr} {}
//
SlimSerialExecutor::PriorityTimer::~PriorityTimer() {}
//
void SlimSerialExecutor::PriorityTimer::Start() {
  if (!PTimerUPtr)
    PTimerUPtr =
        std::make_unique<PriorityTimerImpl>(std::move(PriorityTimerImpl()));
  PTimerUPtr->Start();
}
//
void SlimSerialExecutor::PriorityTimer::Stop() {
  if (!PTimerUPtr)
    return;
  PTimerUPtr->Stop();
  PTimerUPtr.reset();
}
//
void SlimSerialExecutor::PriorityTimer::Resume() {
  if (!PTimerUPtr)
    return;
  PTimerUPtr->Resume();
}
//
void SlimSerialExecutor::PriorityTimer::Pause() {
  if (!PTimerUPtr)
    return;
  PTimerUPtr->Pause();
}
//
void SlimSerialExecutor::PriorityTimer::Pause(
    const std::chrono::microseconds &timeoffMicroSec) {
  if (!PTimerUPtr)
    return;
  PTimerUPtr->Pause(timeoffMicroSec);
}
//
void SlimSerialExecutor::PriorityTimer::AddRealTimeTask(TaskItem_Ty &&titem) {
  if (!PTimerUPtr)
    return;
  PTimerUPtr->AddRealTimeTask(std::move(titem));
}
//
void SlimSerialExecutor::PriorityTimer::AddNormalTask(
    TaskItem_Ty &&titem, Duration_Ty delayMicroSec) {
  if (!PTimerUPtr)
    return;
  PTimerUPtr->AddNormalTask(std::move(titem), delayMicroSec);
}
//
[[nodiscard]] SlimSerialExecutor::RepeatedTaskId_Ty
SlimSerialExecutor::PriorityTimer::AddNormalTask(
    TaskItem_Ty &&titem, const std::chrono::microseconds &delayMicroSec,
    const std::chrono::microseconds &periodMicroSec, uint64_t repCount) {
  if (!PTimerUPtr)
    return;
  PTimerUPtr->AddNormalTask(std::move(titem), delayMicroSec, periodMicroSec,
                            repCount);
}
//
bool SlimSerialExecutor::PriorityTimer::TryCancelSpecifiedTask(
    RepeatedTaskId_Ty rtid) {
  if (!PTimerUPtr)
    return;
  PTimerUPtr->TryCancelTask(rtid);
}
//
void SlimSerialExecutor::PriorityTimer::DeleteAllTasks() {
  if (!PTimerUPtr)
    return;
  PTimerUPtr->DeleteAllTask();
}
} // namespace CustomerDefined