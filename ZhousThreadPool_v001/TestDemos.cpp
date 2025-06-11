//
#include "ZhousThreadPool.h"
//
#include "SlimExecutors.h"
//
#ifdef _DEBUG
#include <iostream>
#include <sstream>
//
std::string GetCurrentDataTimeString() {
  char buf[64];
  time_t nowTimet = time(nullptr);
  tm nowTm;
  localtime_s(&nowTm, &nowTimet);
  snprintf(buf, sizeof(buf), "[%04d_%02d_%02d] [%02d:%02d:%02d]",
           nowTm.tm_year + 1900, nowTm.tm_mon + 1, nowTm.tm_mday, nowTm.tm_hour,
           nowTm.tm_min, nowTm.tm_sec);
  return std::string(buf);
}
//
std::string GetThisThreadIdString() {
  static std::ostringstream oss;
  oss.str("");
  oss.clear();
  oss << std::this_thread::get_id();
  return oss.str();
}

//
#define DebugReportToConsole(msg)                                              \
  do {                                                                         \
    std::cout << "\n[线程: " << std::this_thread::get_id() << " ] "            \
              << GetCurrentDataTimeString() << "\n[调试信息:" << (msg)         \
              << "...]";                                                       \
  } while (0)
#else // !_DEBUG
//
#define DebugReportToConsole(msg)

#endif // _DEBUG
//
void TestTFuncNoResNoArg() {
  DebugReportToConsole("Call TestTFuncNoResNoArg,休眠 300 milliseconds");
  std::this_thread::sleep_for(std::chrono::milliseconds(300));
  DebugReportToConsole("Exit TestTFuncNoResNoArg");
}
//
void TestTFuncNoResWithArg(size_t millisec) {
  DebugReportToConsole(std::string("Call TestTFuncNoResWithArg,休眠 ") +
                       std::to_string(millisec) + " milliseconds");
  std::this_thread::sleep_for(std::chrono::milliseconds(millisec));
  DebugReportToConsole("Exit TestTFuncNoResWithArg");
}
//
size_t TestTFuncWithResNoArg() {
  DebugReportToConsole("Call TestTFuncNoResNoArg,休眠 500 milliseconds");
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  DebugReportToConsole("Exit TestTFuncNoResNoArg");
  return 500;
}
//
size_t TestTFuncWithResWithArg(size_t millisec) {
  DebugReportToConsole(std::string("Call TestTFuncWithResWithArg,休眠 ") +
                       std::to_string(millisec) + " milliseconds");
  std::this_thread::sleep_for(std::chrono::milliseconds(millisec));
  DebugReportToConsole("Exit TestTFuncWithResWithArg");
  return millisec;
}

int Test01() {
  using namespace CustomerDefined;
  try {
    const std::string mainPrompt{std::string("\n[MainThread,") +
                                 GetThisThreadIdString() + "] |>"};
    ZhousThreadPool TestThreadPool(4);
    std::cout << mainPrompt << "线程池当前快照:\n"
              << TestThreadPool.GetSnapshot().ToString(true);
    //
    TestThreadPool.Start();
    std::cout << mainPrompt << "线程池当前快照:\n"
              << TestThreadPool.GetSnapshot().ToString(true);
    //
    std::thread TaskPrincipalColl[4];
    TaskPrincipalColl[0] = std::thread([&TestThreadPool]() {
      for (size_t cnt = 0; cnt < 6; ++cnt) {
        TestThreadPool.AddNoReturnTaskItem(TestTFuncNoResNoArg);
      }
      std::cout << "\n[子线程, " << std::this_thread::get_id()
                << " ]:\n添加任务(6)成功,线程池快照:\n"
                << TestThreadPool.GetSnapshot().ToString(true);
    });
    //
    TaskPrincipalColl[1] = std::thread([&TestThreadPool]() {
      for (size_t cnt = 0; cnt < 6; ++cnt) {

        TestThreadPool.AddNoReturnTaskItem(TestTFuncNoResWithArg, cnt * 500);
      }
      std::cout << "\n[子线程, " << std::this_thread::get_id()
                << " ]:\n添加任务(6)成功,线程池快照:\n"
                << TestThreadPool.GetSnapshot().ToString(true);
    });
    //
    std::future<size_t> t3ResColl[6];
    TaskPrincipalColl[2] = std::thread([&TestThreadPool, &t3ResColl]() {
      for (size_t cnt = 0; cnt < 6; ++cnt) {
        t3ResColl[cnt] =
            std::move(TestThreadPool.AddTaskItem(TestTFuncWithResNoArg));
      }
      std::cout << "\n[子线程, " << std::this_thread::get_id()
                << " ]:\n添加任务(6)成功,线程池快照:\n"
                << TestThreadPool.GetSnapshot().ToString(true);
    });
    //
    std::future<size_t> t4ResColl[6];
    TaskPrincipalColl[3] = std::thread([&TestThreadPool, &t4ResColl]() {
      for (size_t cnt = 0; cnt < 6; ++cnt) {
        t4ResColl[cnt] = std::move(
            TestThreadPool.AddTaskItem(TestTFuncWithResWithArg, cnt * 500));
      }
      std::cout << "\n[子线程, " << std::this_thread::get_id()
                << " ]:\n添加任务(6)成功,线程池快照:\n"
                << TestThreadPool.GetSnapshot().ToString(true);
    });
    //
    for (size_t cnt = 0; cnt < 4; ++cnt) {
      if (TaskPrincipalColl[cnt].joinable())
        TaskPrincipalColl[cnt].join();
    }
    //
    for (size_t cnt = 0; cnt < 20; ++cnt) {
      std::cout << mainPrompt << "[ " << cnt << " ]...线程池当前快照:\n"
                << TestThreadPool.GetSnapshot().ToString(true);
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    //
    std::cout << mainPrompt << " t3 任务结果:\n";
    for (size_t cnt = 0; cnt < 6; ++cnt) {
      std::cout << t3ResColl[cnt].get() << " ; ";
    }
    //
    std::cout << mainPrompt << " t4 任务结果:\n";
    for (size_t cnt = 0; cnt < 6; ++cnt) {
      std::cout << t4ResColl[cnt].get() << " ; ";
    }
    //
    std::cout << mainPrompt << "线程池即将关闭...";
    std::cout << mainPrompt << "线程池当前快照:\n"
              << TestThreadPool.GetSnapshot().ToString(true);
    //
    TestThreadPool.Stop();
    std::cout << mainPrompt << "线程池已关闭...";
    std::cout << mainPrompt << "线程池当前快照:\n"
              << TestThreadPool.GetSnapshot().ToString(true);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  } catch (...) {
    std::cout << "\nTest01 检测到异常...";
    return -1;
  }
  return 0;
}
//
int Test02() {
  using namespace CustomerDefined;
  try {
    const std::string mainPrompt{std::string("\n[MainThread,") +
                                 GetThisThreadIdString() + "] |>"};
    std::cout << mainPrompt << "Test 02 Started...";
    SlimSerialExecutor serialPriorityExecutor;
    serialPriorityExecutor.Start();
    //
    const size_t TestCount = 16;
    std::future<size_t> resColl1[TestCount];
    auto TFunc1 = [&serialPriorityExecutor, TestCount, &resColl1]() {
      std::cout << "[Thread" << std::this_thread::get_id() << "] 添加 "
                << TestCount << " 个延时任务到SlimSerialExecutor";
      for (size_t cnt = 0; cnt < TestCount; ++cnt) {
        resColl1[cnt] = serialPriorityExecutor.PostNormalTask(
            SlimSerialExecutor::Duration_Ty((cnt + 1) * 100),
            TestTFuncWithResWithArg, (cnt + 1) * 100);
      }
      std::cout << "[Thread" << std::this_thread::get_id() << "] 添加 "
                << TestCount << " 个 延时 任务到SlimSerialExecutor 【完成】";
    };
    //
    std::future<size_t> resColl2[TestCount];
    auto TFunc2 = [&serialPriorityExecutor, TestCount, &resColl2]() {
      std::cout
          << "[Thread" << std::this_thread::get_id()
          << "] 每隔 100 milliseconds 添加 1 个 实时 任务到SlimSerialExecutor";
      for (size_t cnt = 0; cnt < TestCount; ++cnt) {
        resColl2[cnt] = serialPriorityExecutor.PostRealTimeTask(
            TestTFuncWithResWithArg, 500);
      }
      std::cout << "[Thread" << std::this_thread::get_id() << "] 添加 "
                << TestCount << " 个 实时 任务到SlimSerialExecutor 【完成】";
    };
    //
    std::thread t1{TFunc1};
    std::thread t2{TFunc2};
    //
    for (size_t cnt = 0; cnt < 3; ++cnt) {
      std::cout << mainPrompt << cnt << "...";
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    serialPriorityExecutor.Pause();
    for (size_t cnt = 0; cnt < 10; ++cnt) {
      std::cout << mainPrompt << cnt << "...";
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    serialPriorityExecutor.Resume();
    //
    if (t1.joinable())
      t1.join();
    if (t2.joinable())
      t2.join();
    //
    std::cout << mainPrompt << "打印带返回值的future结果1:";
    for (size_t cnt = 0; cnt < TestCount; ++cnt) {
      std::cout << mainPrompt << "[ " << cnt << " ]:" << resColl1[cnt].get();
    }
    //
    std::cout << mainPrompt << "打印带返回值的future结果2:";
    for (size_t cnt = 0; cnt < TestCount; ++cnt) {
      std::cout << mainPrompt << "[ " << cnt << " ]:" << resColl2[cnt].get();
    }
    //
    // serialPriorityExecutor.Stop();
    //
    std::cout << mainPrompt << "Test 02 Ended...";
  } catch (...) {
    std::cout << "\nTest02 检测到异常...";
    return -1;
  }
  return 0;
}

//
int main() {
  std::cout << "\nTests Start...";
  //
  // std::cout << ((Test01() == 0) ? "\nTest01 Successed..." : "Test01
  // Failed...");
  //
  std::cout << ((Test02() == 0) ? "\nTest02 Successed..." : "Test02 Failed...");

  //
  std::cout << "\nAll tests done, press any key to exit...";
  std::cin.get();
  return 0;
}