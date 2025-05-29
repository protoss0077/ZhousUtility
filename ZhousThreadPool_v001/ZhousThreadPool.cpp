#include "ZhousThreadPool.h"

namespace CustomerDefined {
/************************************
 * class ZhousThreadPool£¨∂®“Â *
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
    : ThreadSPtrColl{}, TaskItemQueue{}, InternalMutex{}, CondVar{},
      WorkingThreadCount{0}, InPoolThreadCount{0},
      ThreadCountLimit{ConfirmThreadCount(expectCount)}, InitTag{false},
      RunningTag{false} {}

// private members
unsigned int ZhousThreadPool::ConfirmThreadCount(unsigned int expectCount) {
  if (expectCount>MaxThreadCountLimit||expectCount<MinThreadCountLimit)
    return MinThreadCountLimit;
  return expectCount;
}

} // namespace CustomerDefined