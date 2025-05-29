#include "ZhousThreadPool.h"

namespace CustomerDefined {
/************************************
 * class ZhousThreadPool£¨∂®“Â *
 ************************************
 */
// static members
const unsigned int ZhousThreadPool::MaxThreadCountLimit =
    std::thread::hardware_concurrency()*2;
//
const unsigned int ZhousThreadPool::MinThreadCountLimit=1;


} // namespace CustomerDefined