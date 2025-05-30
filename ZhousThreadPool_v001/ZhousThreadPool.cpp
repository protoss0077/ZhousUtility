#include "ZhousThreadPool.h"

namespace CustomerDefined {
/************************************
 * class ZhousThreadPool，定义 *
 ************************************
 */
// static members
const unsigned int ZhousThreadPool::MaxThreadCountLimit =
    std::thread::hardware_concurrency() * 2;
//
const unsigned int ZhousThreadPool::MinThreadCountLimit =
    (std::thread::hardware_concurrency() < 2) ? 1 : 2;
// public members
explicit ZhousThreadPool::ZhousThreadPool(unsigned int expectCount)
    : ThreadSPtrColl{std::move(std::vector<ThreadWapperSPtr_Ty>(
          ConfirmThreadCount(expectCount), nullptr))},
      TaskItemQueue{}, InternalMutex{}, CondVar{}, WorkingThreadCount{0},
      InPoolThreadCount{0}, ThreadCountLimit{ConfirmThreadCount(expectCount)},
      RunningTag{false} {}
//
ZhousThreadPool::~ZhousThreadPool() { Stop(); }
//
void ZhousThreadPool::Start() {
  if (RunningTag.load())
    return;
  auto twSPtr = std::make_shared<ThreadWapper>([this]() { DefThreadFunc(); });
  ThreadSPtrColl[InPoolThreadCount++] = std::move(twSPtr);
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
}
//
unsigned int ZhousThreadPool::TryAddThread() {
  if (!RunningTag)
    return 0;
  //
  if (InPoolThreadCount.load() >= ThreadCountLimit)
    return ThreadCountLimit;
  // 按设想此时仍有空闲的线程，任务队列必然为空
  if (WorkingThreadCount.load() < InPoolThreadCount.load())
    return InPoolThreadCount.load();
  //
  auto twSPtr = std::make_shared<ThreadWapper>([this]() { DefThreadFunc(); });
  ThreadSPtrColl[InPoolThreadCount++] = std::move(twSPtr);
  return InPoolThreadCount.load();
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
}
} // namespace CustomerDefined