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
  std::atomic<State_Ty> State;
  // 重复任务编号
  std::atomic<RepeatedTaskId_Ty> ValidRepeatedTaskId;
  //
  PriorityTimerImpl()
      : TaskItemPQueue{}, RepeatedIdUSet{}, TaskRunnerUPtr{nullptr},
        InternalMutex{}, CondVar{}, State{State_Ty::Stopped},
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
  RepeatedTaskId_Ty GetRepeatedId() {
    if (ValidRepeatedTaskId == InvalidRepeatedTaskId)
      throw std::runtime_error("生成了无效的任务ID");
    return ValidRepeatedTaskId++;
  }
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
  if (State.load() != State_Ty::Stopped)
    return;
  State.store(State_Ty::Running);
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
    if (State.load() == State_Ty::Stopped)
      return;
    for (; !TaskItemPQueue.empty();)
      TaskItemPQueue.pop();
    RepeatedIdUSet.clear();
    ValidRepeatedTaskId.store(0);
    State.store(State_Ty::Stopped);
  } // lk
  CondVar.notify_one();
  TaskRunnerUPtr.reset();
}
//
void SlimSerialExecutor::PriorityTimerImpl::Pause() {
  std::lock_guard<std::mutex> lk(InternalMutex);
  if (State.load() != State_Ty::Running)
    return;
  State.store(State_Ty::Paused);
}
//
void SlimSerialExecutor::PriorityTimerImpl::Pause(Duration_Ty timeOffMicroSec) {
  AddRealTimeTask([this, timeOffMicroSec]() {
    {
      std::lock_guard<std::mutex> lk(this->InternalMutex);
      if (State.load() != State_Ty::Running)
        return;
      State.store(State_Ty::Paused);
    } // lk
    std::this_thread::sleep_for(timeOffMicroSec);
    {
      std::lock_guard<std::mutex> lk(this->InternalMutex);
      if (State.load() != State_Ty::Paused)
        return;
      State.store(State_Ty::Running);
    } // lk
  });
}
//
void SlimSerialExecutor::PriorityTimerImpl::Resume() {
  bool notifyTag = true;
  {
    std::lock_guard<std::mutex> lk(InternalMutex);
    if (State.load() != State_Ty::Paused)
      return;
    notifyTag = !TaskItemPQueue.empty();
    State.store(State_Ty::Running);
  } // lk
  if (notifyTag)
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
    if (State.load() == State_Ty::Stopped)
      return;
    notifyTag =
        (TaskItemPQueue.empty() ? true
                                : startTp <= TaskItemPQueue.top().StartTp) &&
        (State.load() == State_Ty::Running);
    TaskItemPQueue.push(std::move(PriorityTaskItem(startTp, std::move(titem))));
  } // lk
  if (notifyTag)
    CondVar.notify_one();
}
//
[[nodiscard]] SlimSerialExecutor::RepeatedTaskId_Ty
SlimSerialExecutor::PriorityTimerImpl::AddNormalTask(TaskItem_Ty &&titem,
                                                     Duration_Ty delayMicroSec,
                                                     Duration_Ty periodMicroSec,
                                                     uint64_t repCount) {
  RepeatedTaskId_Ty resId = InvalidRepeatedTaskId;
  {
    std::lock_guard<std::mutex> lk(InternalMutex);
    if (State.load() == State_Ty::Stopped)
      return;
    resId = GetRepeatedId();
    RepeatedIdUSet.insert(resId);
  } // lk
  PostRepeatedTask(std::move(titem), resId, delayMicroSec, periodMicroSec,
                   repCount);
  return resId;
}
//
bool SlimSerialExecutor::PriorityTimerImpl::TryCancelTask(
    RepeatedTaskId_Ty rtid) {
  std::lock_guard<std::mutex> lk(InternalMutex);
  if (State.load() == State_Ty::Stopped)
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
      return State.load() == State_Ty::Stopped ||
             (!TaskItemPQueue.empty() && State == State_Ty::Running);
    });
    if (State.load() == State_Ty::Stopped)
      break;
    if (State.load() == State_Ty::Paused) {
      CondVar.wait(ulk, [this]() { return State.load() != State_Ty::Paused; });
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
  auto postFunc = [this, &titem, rtid, periodMicroSec, repCount]() {
    this->ExecuteAndAddNext(std::move(titem), rtid, periodMicroSec, repCount);
  };
  {
    std::lock_guard<std::mutex> lk(InternalMutex);
    auto fIter = RepeatedIdUSet.find(rtid);
    if (fIter == RepeatedIdUSet.end())
      return;
    if (repCount == 0) {
      RepeatedIdUSet.erase(fIter);
      return;
    }
    notifyTag =
        (TaskItemPQueue.empty() ? true
                                : startTp <= TaskItemPQueue.top().StartTp) &&
        (State.load() == State_Ty::Running);
    TaskItemPQueue.push(
        std::move(PriorityTaskItem(startTp, std::move(postFunc), rtid)));
  } // lk
  if (notifyTag)
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
  if (notifyTag && State.load() == State_Ty::Running)
    CondVar.notify_one();
}
/************************************
 * SlimSerialExecutor，定义 *
 * 外部接口封装 *
 ************************************
 */
SlimSerialExecutor::SlimSerialExecutor() : PTimerUPtr{nullptr} {}
//
SlimSerialExecutor::~SlimSerialExecutor() {}
//
void SlimSerialExecutor::Start() {
  if (!PTimerUPtr)
    PTimerUPtr =
        std::make_unique<PriorityTimerImpl>(std::move(PriorityTimerImpl()));
  PTimerUPtr->Start();
}
//
void SlimSerialExecutor::Stop() {
  if (!PTimerUPtr)
    return;
  PTimerUPtr->Stop();
  PTimerUPtr.reset();
}
//
void SlimSerialExecutor::Resume() {
  if (!PTimerUPtr)
    return;
  PTimerUPtr->Resume();
}
//
void SlimSerialExecutor::Pause() {
  if (!PTimerUPtr)
    return;
  PTimerUPtr->Pause();
}
//
void SlimSerialExecutor::Pause(
    const std::chrono::microseconds &timeoffMicroSec) {
  if (!PTimerUPtr)
    return;
  PTimerUPtr->Pause(timeoffMicroSec);
}
//
void SlimSerialExecutor::AddRealTimeTask(TaskItem_Ty &&titem) {
  if (!PTimerUPtr)
    return;
  PTimerUPtr->AddRealTimeTask(std::move(titem));
}
//
void SlimSerialExecutor::AddNormalTask(TaskItem_Ty &&titem,
                                       Duration_Ty delayMicroSec) {
  if (!PTimerUPtr)
    return;
  PTimerUPtr->AddNormalTask(std::move(titem), delayMicroSec);
}
//
[[nodiscard]] SlimSerialExecutor::RepeatedTaskId_Ty
SlimSerialExecutor::AddNormalTask(
    TaskItem_Ty &&titem, const std::chrono::microseconds &delayMicroSec,
    const std::chrono::microseconds &periodMicroSec, uint64_t repCount) {
  if (!PTimerUPtr)
    return;
  PTimerUPtr->AddNormalTask(std::move(titem), delayMicroSec, periodMicroSec,
                            repCount);
}
//
bool SlimSerialExecutor::TryCancelSpecifiedTask(RepeatedTaskId_Ty rtid) {
  if (!PTimerUPtr)
    return;
  PTimerUPtr->TryCancelTask(rtid);
}
//
void SlimSerialExecutor::DeleteAllTasks() {
  if (!PTimerUPtr)
    return;
  PTimerUPtr->DeleteAllTask();
}
/************************************
 * class SlimExecutor::SlimExecutorImpl，定义 *
 ************************************
 */
class SlimExecutor::SlimExecutorImpl {

public:
  // data member
  TaskRunnerUPtr_Ty RunnerUPtr;
  std::queue<TaskItem_Ty> TaskItemQueue;
  //
  std::mutex InternalMutex;
  std::condition_variable CondVar;
  std::atomic<State_Ty> State;
  //
public:
  SlimExecutorImpl();
  ~SlimExecutorImpl();
  // 对外接口
  void Start();
  //
  void Stop();
  //
  void Resume();
  //
  void Pause();
  //
  void AddTask(TaskItem_Ty &&titem);
  //
  void ThreadFunc();
}; // class SlimExecutor::SlimExecutorImpl
//
SlimExecutor::SlimExecutorImpl::SlimExecutorImpl()
    : RunnerUPtr{nullptr}, TaskItemQueue{}, InternalMutex{}, CondVar{},
      State{State_Ty::Stopped} {}
//
SlimExecutor::SlimExecutorImpl::~SlimExecutorImpl() { Stop(); }
//
void SlimExecutor::SlimExecutorImpl::Start() {
  std::lock_guard<std::mutex> lk(InternalMutex);
  if (State.load() != State_Ty::Stopped)
    return;
  State.store(State_Ty::Running);
  for (; !TaskItemQueue.empty();)
    TaskItemQueue.pop();
  RunnerUPtr =
      std::make_unique<TaskRunner_Ty>([this]() { this->ThreadFunc(); });
}
//
void SlimExecutor::SlimExecutorImpl::Stop() {
  {
    std::lock_guard<std::mutex> lk(InternalMutex);
    if (State.load() == State_Ty::Stopped)
      return;
    for (; !TaskItemQueue.empty();)
      TaskItemQueue.pop();
    State.store(State_Ty::Stopped);
  }
  CondVar.notify_one();
  RunnerUPtr.reset();
}
//
void SlimExecutor::SlimExecutorImpl::Resume() {
  {
    std::lock_guard<std::mutex> lk(InternalMutex);
    if (State.load() != State_Ty::Paused)
      return;
    State.store(State_Ty::Running);
  }
  CondVar.notify_one();
}
//
void SlimExecutor::SlimExecutorImpl::Pause() {
  std::lock_guard<std::mutex> lk(InternalMutex);
  if (State.load() != State_Ty::Running)
    return;
  State.store(State_Ty::Paused);
}
//
void SlimExecutor::SlimExecutorImpl::AddTask(TaskItem_Ty &&titem) {
  bool notifyTag = true;
  {
    std::lock_guard<std::mutex> lk(InternalMutex);
    if (State.load() == State_Ty::Stopped)
      return;
    notifyTag = State.load() != State_Ty::Paused;
    TaskItemQueue.push(std::move(titem));
  } // lk
  if (notifyTag)
    CondVar.notify_one();
}
//
void SlimExecutor::SlimExecutorImpl::ThreadFunc() {
  for (;;) {
    std::unique_lock<std::mutex> ulk(InternalMutex);
    CondVar.wait(ulk, [this]() {
      return State.load() == State_Ty::Stopped ||
             (!TaskItemQueue.empty() && State == State_Ty::Running);
    });
    if (State.load() == State_Ty::Stopped)
      break;
    if (State.load() == State_Ty::Paused) {
      CondVar.wait(ulk, [this]() { return State.load() != State_Ty::Paused; });
      continue;
    }
    if (TaskItemQueue.empty())
      continue;

    auto pitem = TaskItemQueue.top();
    ulk.unlock();
    pitem();
  } // for-loop
}
/************************************
 * class SlimExecutor::SlimExecutorImpl，定义 *
 ************************************
 */
SlimExecutor::SlimExecutor() : ExecutorUPtr{nullptr} {}
//
SlimExecutor::~SlimExecutor() {
  if (!ExecutorUPtr)
    return;
  ExecutorUPtr->Stop();
}
//
void SlimExecutor::Start() {
  if (!ExecutorUPtr)
    return;
  ExecutorUPtr->Start();
}
//
void SlimExecutor::Stop() {
  if (!ExecutorUPtr)
    return;
  ExecutorUPtr->Stop();
}
//
bool SlimExecutor::IsRunning() const {
  if (!ExecutorUPtr)
    return false;
  return (ExecutorUPtr->State.load()) == State_Ty::Running;
}
//
void SlimExecutor::Resume() {
  if (!ExecutorUPtr)
    return;
  ExecutorUPtr->Resume();
}
//
void SlimExecutor::Pause() {
  if (!ExecutorUPtr)
    return;
  ExecutorUPtr->Pause();
}
//
void SlimExecutor::AddTask(TaskItem_Ty &&titem) {
  if (!ExecutorUPtr)
    return;
  ExecutorUPtr->AddTask(std::move(titem));
}
} // namespace CustomerDefined