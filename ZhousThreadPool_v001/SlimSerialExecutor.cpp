#include "SlimSerialExecutor.h"

namespace CustomerDefined {
/************************************************
 * SlimSerialExecutor::PriorityTimer ，定义 *
 ************************************************
 */
SlimSerialExecutor::PriorityTimer::PriorityTimer()
    : TaskItemPQueue{}, RepeatedTaskIdUSet{}, InternalThreadUPtr{nullptr},
      InternalMutex{}, CondVar{}, RunningTag{false}, PausedTag{false},
      RepeatedTaskId{0} {}
//
SlimSerialExecutor::PriorityTimer::~PriorityTimer() { Stop(); }
//
void SlimSerialExecutor::PriorityTimer::Start() {
  if (RunningTag.load())
    return;
  //
  for (; !TaskItemPQueue.empty();)
    TaskItemPQueue.pop();
  RepeatedTaskIdUSet.clear();
  //
  RepeatedTaskId.store(0);
  InternalThreadUPtr =
      std::make_unique<ThreadWapper>([this]() { this->DefThreadFunc(); });
  RunningTag.store(true);
  PausedTag.store(false);
}
//
void SlimSerialExecutor::PriorityTimer::ReStart() {
  if (RunningTag.load() || PausedTag.load())
    return;
  PausedTag.store(false);
  CondVar.notify_one();
}
//
void SlimSerialExecutor::PriorityTimer::Pause() {
  if (RunningTag.load() || !PausedTag.load())
    return;
  PausedTag.store(true);
}
//
void SlimSerialExecutor::PriorityTimer::Pause(
    const std::chrono::microseconds &timeoffMicroSec) {
  AddRealTimeTask([this, timeoffMicroSec]() { StayAWhile(timeoffMicroSec); });
}
//
void SlimSerialExecutor::PriorityTimer::AddRealTimeTask(TaskItem_Ty titem) {
  // 这里我没加锁，如果加锁需要递归锁？
  auto nowTp = Clock_Ty::now();
  auto topItemTp = TaskItemPQueue.top().StartTp;

  auto duMicroSec =
      (nowTp > topItemTp)
          ? std::chrono::duration_cast<std::chrono::microseconds>(
                topItemTp - nowTp - std::chrono::microseconds(100))
          : std::chrono::microseconds(-1);
  AddNormalTask(titem, duMicroSec);
}
//
void SlimSerialExecutor::PriorityTimer::AddNormalTask(
    TaskItem_Ty titem, const std::chrono::microseconds &delayedMicroSec) {
  auto startTp = Clock_Ty::now() + delayedMicroSec;
  std::unique_lock<std::mutex> ulk(InternalMutex);
  TaskItemPQueue.push(std::move(PriorityTaskItem(startTp, std::move(titem))));
  CondVar.notify_one();
}
//
[[nodiscard]] uint64_t SlimSerialExecutor::PriorityTimer::AddNormalTask(
    TaskItem_Ty titem, const std::chrono::microseconds &delayedMicroSec,
    const std::chrono::microseconds &periodMicroSec, uint64_t repCount) {
  RepeatedTaskId_Ty curTaskId = GetNextRepeatedTaskId();
  RepeatedTaskIdUSet.insert(curTaskId);
  InternalAddNormalTask(std::move(titem), curTaskId, delayedMicroSec,
                        periodMicroSec, repCount);
  return curTaskId;
}

// private members
void SlimSerialExecutor::PriorityTimer::DefThreadFunc() {}
//
void SlimSerialExecutor::PriorityTimer::StayAWhile(
    const std::chrono::microseconds &timeoffMicroSec) {
  PausedTag.store(true);
  std::this_thread::sleep_for(timeoffMicroSec);
  PausedTag.store(false);
}
//
SlimSerialExecutor::RepeatedTaskId_Ty
SlimSerialExecutor::PriorityTimer::GetNextRepeatedTaskId() {
  return RepeatedTaskId++;
}
//
void SlimSerialExecutor::PriorityTimer::InternalAddNormalTask(
    TaskItem_Ty &&titem, RepeatedTaskId_Ty rtid,
    const std::chrono::microseconds &delayedMicroSec,
    const std::chrono::microseconds &periodMicroSec, uint64_t repCount) {
  if (RepeatedTaskIdUSet.find(rtid) == RepeatedTaskIdUSet.end())
    return;
  if (repCount == 0) {
    RepeatedTaskIdUSet.erase(rtid);
    return;
  }
  //
  TaskItem_Ty transTaskItem = [this, &titem, rtid, periodMicroSec, repCount]() {
    this->TaskExecuteAndAddNext(std::move(titem), rtid, periodMicroSec,
                                repCount);
  };
  auto transStartTp = Clock_Ty::now() + delayedMicroSec;
  {
    std::unique_lock<std::mutex> ulk(InternalMutex);
    TaskItemPQueue.push(
        std::move(PriorityTaskItem(transStartTp, std::move(transTaskItem))));
  }
  CondVar.notify_one();
}
//
void SlimSerialExecutor::PriorityTimer::TaskExecuteAndAddNext(
    TaskItem_Ty &&titem, RepeatedTaskId_Ty rtid,
    const std::chrono::microseconds &periodMicroSec, uint64_t repCount) {
  titem();
  InternalAddNormalTask(titem,rtid,periodMicroSec,periodMicroSec,repCount-1);
}
//
} // namespace CustomerDefined