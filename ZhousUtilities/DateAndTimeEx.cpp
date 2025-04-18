#include "ZUCmdAssistants.h"

namespace ZhousUtilities {
namespace ZUMCAssistants {
//
tm GetCurrentTM() {
  time_t t = time(0);
  tm now;
  localtime_s(&now, &t);
  return now;
}
//
std::string TimeRecorder::Elapsed() const {
  if (m_StartTime == Clock_Ty::time_point::min()) {
    throw std::runtime_error("TimeRecorder::Elapsed() called without Start()");
  }
  auto endTime = Clock_Ty::now();
  auto duration =
      std::chrono::duration_cast<Duration_Ty>(endTime - m_StartTime);
  return std::to_string(duration.count()) + " nanosec";
}
//
} // namespace ZUMCAssistants
} // namespace ZhousUtilities