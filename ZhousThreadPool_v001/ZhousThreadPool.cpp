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
      InitTag{false}, RunningTag{false} {}

// private members
unsigned int ZhousThreadPool::ConfirmThreadCount(unsigned int expectCount) {
  if (expectCount > MaxThreadCountLimit || expectCount < MinThreadCountLimit)
    return MinThreadCountLimit;
  return expectCount;
}
//
void ZhousThreadPool::DefThreadFunc() { 
  for (;;) {

  } // for-loop
}
/************************************
 * struct ThreadWapper，定义 *
 ************************************
 */
ThreadWapper::ThreadWapper(std::function<void(void)> &&tfunc)
    : InternalThread{std::move(tfunc)} {}
//
ThreadWapper::~ThreadWapper() {
  if (InternalThread.joinable())
    InternalThread.join();
}

} // namespace CustomerDefined