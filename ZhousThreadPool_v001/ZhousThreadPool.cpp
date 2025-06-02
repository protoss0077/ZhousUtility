#include "ZhousThreadPool.h"
//
#include <chrono>
//
namespace CustomerDefined {
/************************************
 * class ZhousThreadPool::struct PoolSnapshot，定义 *
 ************************************
 */
ZhousThreadPool::PoolSnapshot::PoolSnapshot(bool rtag, unsigned int wtc,
                                            unsigned int itc, unsigned int tcl,
                                            size_t tiiq)
    : RunningTag{rtag}, WorkingThreadCount{wtc}, InPoolThreadCount{itc},
      ThreadCountLimit{tcl}, TaskItemInQueue{tiiq} {}
//
std::string ZhousThreadPool::PoolSnapshot::ToString(bool tabViewTag) const {
  if (!RunningTag)
    return "未运行...";
  if (tabViewTag && ThreadCountLimit < 10)
    return ToTableString();
  return ToDefString();
}
//
std::string ZhousThreadPool::PoolSnapshot::ToTableString() const {
  std::string resStr;
  resStr.reserve(64);
  resStr.append("正在运行:|");
  for (unsigned int cnt = 0; cnt < WorkingThreadCount; ++cnt) {
    if (cnt == MinThreadCountLimit)
      resStr.append("!");
    resStr.append(std::to_string(cnt));
    resStr.append(" * ");
  }
  //
  for (unsigned int cnt = WorkingThreadCount; cnt < InPoolThreadCount; ++cnt) {
    if (cnt == MinThreadCountLimit)
      resStr.append("!");
    resStr.append(std::to_string(cnt));
    resStr.append(" _ ");
  }
  //
  for (unsigned int cnt = InPoolThreadCount; cnt < ThreadCountLimit; ++cnt) {
    if (cnt == MinThreadCountLimit)
      resStr.append("!");
    resStr.append(std::to_string(cnt));
    resStr.append(" x ");
  }
  resStr.append(std::to_string(ThreadCountLimit));
  resStr.append("|| ");
  resStr.append(std::to_string(TaskItemInQueue));
  return resStr.append(" |");
}
//
std::string ZhousThreadPool::PoolSnapshot::ToDefString() const {
  std::string resStr;
  resStr.reserve(128);
  resStr.append("线程池正在运行:\n最大线程数: ");
  resStr.append(std::to_string(ThreadCountLimit));
  resStr.append(" ;当前池内线程数: ");
  resStr.append(std::to_string(InPoolThreadCount));
  resStr.append("\n工作: ");
  resStr.append(std::to_string(WorkingThreadCount));
  resStr.append(" ;空闲: ");
  resStr.append(std::to_string(InPoolThreadCount - WorkingThreadCount));
  resStr.append("\n剩余工作项: ");
  return resStr.append(std::to_string(TaskItemInQueue));
}
/************************************
 * class ZhousThreadPool，定义 *
 ************************************
 */
// static members
const unsigned int ZhousThreadPool::MaxThreadCountLimit =
    std::thread::hardware_concurrency() * 2;
//
const unsigned int ZhousThreadPool::MinThreadCountLimit = 1;
// public members
ZhousThreadPool::ZhousThreadPool(unsigned int expectCount)
    : ThreadSPtrColl{}, TaskItemQueue{}, InternalMutex{}, CondVar{},
      WorkingThreadCount{0}, InPoolThreadCount{0},
      ThreadCountLimit{ConfirmThreadCount(expectCount)}, RunningTag{false},
      RemoveState{RemoveStateNone} {
  ThreadSPtrColl.reserve(ThreadCountLimit);
}
//
ZhousThreadPool::~ZhousThreadPool() { Stop(); }
//
void ZhousThreadPool::Start() {
  if (RunningTag.load())
    return;
  auto twSPtr = std::make_shared<ThreadWapper>([this]() { Thread0Func(); });
  ThreadSPtrColl.push_back(std::move(twSPtr));
  InPoolThreadCount++;
  RunningTag.store(true);
  return;
}
//
void ZhousThreadPool::Stop() {
  if (!RunningTag.load())
    return;
  RunningTag.store(false);
  CondVar.notify_all();
  //
  ThreadSPtrColl.clear();
  WorkingThreadCount.store(0);
  InPoolThreadCount.store(0);
  RemoveState.store(RemoveStateNone);
}
//
unsigned int ZhousThreadPool::TryAddThread() {
  if (!RunningTag)
    return 0;
  //
  if (InPoolThreadCount.load() >= ThreadCountLimit)
    return ThreadCountLimit;
  // 按设想此时仍有空闲的线程，任务队列必然为空
  if (WorkingThreadCount.load() < InPoolThreadCount.load() ||
      RemoveState.load() != RemoveStateNone)
    return InPoolThreadCount.load();
  //
  std::lock_guard<std::mutex> lk(InternalMutex);
  if (RemoveState.load() != RemoveStateNone)
    return InPoolThreadCount.load();
  //
  auto twSPtr = std::make_shared<ThreadWapper>([this]() { DefThreadFunc(); });
  ThreadSPtrColl.push_back(std::move(twSPtr));
  InPoolThreadCount++;
  return InPoolThreadCount.load();
}
//
ZhousThreadPool::PoolSnapshot ZhousThreadPool::GetSnapshot() const {
  // std::lock_guard<std::mutex> lk(InternalMutex);
  return PoolSnapshot(RunningTag.load(), WorkingThreadCount.load(),
                      InPoolThreadCount.load(), ThreadCountLimit,
                      TaskItemQueue.size());
}
// private members
unsigned int ZhousThreadPool::ConfirmThreadCount(unsigned int expectCount) {
  if (expectCount > MaxThreadCountLimit)
    return MaxThreadCountLimit;
  if (expectCount < MinThreadCountLimit)
    return MinThreadCountLimit;
  return expectCount;
}
//
void ZhousThreadPool::DefThreadFunc() {
  for (; RunningTag.load();) {
    TaskItem_Ty titem;
    {
      std::unique_lock<std::mutex> ulk(InternalMutex);
      CondVar.wait(ulk, [this]() {
        return !RunningTag.load() || !TaskItemQueue.empty() ||
               RemoveState != RemoveStateNone;
      });
      //
      if (!RunningTag.load())
        break;
      //
      if (RemoveState.load() == RemoveStateLastIdle) {
        auto fPos = 1;
        for (; fPos < ThreadSPtrColl.size(); ++fPos) {
          if (ThreadSPtrColl[fPos]->GetId() == std::this_thread::get_id())
            break;
        }
        std::swap(ThreadSPtrColl[fPos], ThreadSPtrColl.back());
        RemoveState.store(RemoveStateLastEnded);
        break;
      }
      // 伪唤醒
      if (TaskItemQueue.empty())
        continue;
      //
      titem = std::move(TaskItemQueue.front());
      TaskItemQueue.pop();
    }
    WorkingThreadCount++;
    titem();
    WorkingThreadCount--;
  } // for-loop
}
//
void ZhousThreadPool::Thread0Func() {
  for (; RunningTag.load();) {
    TaskItem_Ty titem;
    {
      std::unique_lock<std::mutex> ulk(InternalMutex);
      const auto state = CondVar.wait_for(
          ulk, std::chrono::milliseconds(AutoRemoveTimeOut),
          [this]() { return !RunningTag.load() || !TaskItemQueue.empty(); });
      if (!RunningTag.load()) {
        break;
      }
      if (RemoveState.load() == RemoveStateLastEnded) {
        ThreadSPtrColl.pop_back();
        InPoolThreadCount--;
        RemoveState.store(RemoveStateNone);
      }
      if (state) {
        // 伪唤醒
        if (TaskItemQueue.empty()) {
          continue;
        }
        // 是否需要添加线程
        if (InPoolThreadCount.load() < ThreadCountLimit &&
            (WorkingThreadCount.load() + 1) >= InPoolThreadCount.load()) {
          auto twSPtr =
              std::make_shared<ThreadWapper>([this]() { DefThreadFunc(); });
          ThreadSPtrColl.push_back(std::move(twSPtr));
          InPoolThreadCount++;
        }
        //
        titem = std::move(TaskItemQueue.front());
        TaskItemQueue.pop();
        // 不在锁内执行
        ulk.unlock();
        WorkingThreadCount++;
        titem();
        WorkingThreadCount--;
      } else {
        if (RemoveState.load() == RemoveStateLastIdle)
          continue;
        if (InPoolThreadCount.load() > MinThreadCountLimit &&
            (WorkingThreadCount.load() + 1) < InPoolThreadCount.load()) {
          RemoveState.store(RemoveStateLastIdle);
          ulk.unlock();
          CondVar.notify_one();
        }
      }
    } // ulk
  } // for-loop
}
} // namespace CustomerDefined
