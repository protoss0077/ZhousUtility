#include "SlimSerialExecutor.h"

namespace CustomerDefined {
/************************************************
 * SlimSerialExecutor::PriorityTimer ，定义 *
 ************************************************
 */
SlimSerialExecutor::PriorityTimer::PriorityTimer()
    : TaskItemPQueue{}, RepeatedTaskIdUSet{}, InternalThreadUPtr{nullptr},
      InternalMutex{}, CondVar{}, RunningTag{false}, NextRepeatedTaskId{0} {}
//
SlimSerialExecutor::PriorityTimer::~PriorityTimer() { Stop(); }
//
void SlimSerialExecutor::PriorityTimer::Run() {
  if (RunningTag.load())
    return;
  //
  InternalThreadUPtr =
      std::make_unique<ThreadWapper>([this]() { this->DefThreadFunc(); });
  RunningTag.store(true);
}

// private members
void SlimSerialExecutor::PriorityTimer::DefThreadFunc() {
  for (; RunningTag.load();) {
    std::unique_lock<std::mutex> ulk(InternalMutex);
    CondVar.wait(ulk, [this]() { return !TaskItemPQueue.empty(); });
    if (!RunningTag.load())
      break;
    //
    PriorityTaskItem ptItem = TaskItemPQueue.top();
    const auto diff = ptItem.StartTp - SlimSerialExecutor::Clock_Ty::now();
    if (std::chrono::duration_cast<std::chrono::microseconds>(diff).count() >
        0) {
      CondVar.wait_for(ulk, diff);
      continue;
    }
    //
    TaskItemPQueue.pop();
    // 锁外执行
    ulk.unlock();
    ptItem.TaskItem();
  } // for-loop
}

} // namespace CustomerDefined