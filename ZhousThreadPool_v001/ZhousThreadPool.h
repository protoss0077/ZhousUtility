#pragma once
//
#include <functional>
//
#include <queue>
#include <vector>
//
#include <future>
#include <mutex>
//
namespace CustomerDefined {
// ǰ������
// std::shared_ptr<std::thread>�İ�װ��
// ��д������
struct ThreadSPtrWapper;
// �����ڹ�����
using TaskItem_Ty = std::function<void(void)>;
/* class ZhousThreadPool
 */
class ZhousThreadPool {
public:
  // ����߳�������
  static const unsigned int MaxThreadCountLimit;
  // ��С�߳�������
  static const unsigned int MinThreadCountLimit;

private:
  std::vector<ThreadSPtrWapper> ThreadSPtrColl;
  std::queue<TaskItem_Ty> TaskItemQueue;
  //
  std::mutex InternalMutex;
  std::condition_variable CondVar;
  /* �������/��С��������
   * ���ӵ�ǰ�����������߳�����
   */
  std::atomic<unsigned int> WorkingThreadCount;
  std::atomic<unsigned int> InPoolThreadCount;
  // ��ֵ�����/��С/�û�����ֵȷ����ֻ��
  const unsigned int ThreadCountLimit;
  // ��ʼ����� true/false �ѳ�ʼ��/δ��ʼ��
  std::atomic<bool> InitTag;
  // ���б�� true/false
  std::atomic<bool> RunningTag;

public:
  //
  explicit ZhousThreadPool(unsigned int expectTCount);
  //
  ZhousThreadPool(const ZhousThreadPool &) = delete;
  //
  ~ZhousThreadPool();
  //
  ZhousThreadPool &operator=(const ZhousThreadPool &) = delete;
  //
  bool Start();
  //
  void Stop();
  /* AddThread
   * ��������ʱΪ�̳߳����һ�������߳�
   * ������
   * InPoolThreadCountС��MaxThreadCountLimit
   * �Ҷ��в�Ϊ��
   * ��WorkingThreadCount == InPoolThreadCount
   * ����InPoolThreadCount�ĵ�ǰֵ
   */
  unsigned int AddThread();
  /* Remove
   * ��������ʱ�Ƴ�һ�������߳�
   * ����:
   * InPoolThreadCount����MinThreadCountLimit
   * �Ҷ���Ϊ��,��ʱ��Ӧ��WorkingThread(==0)
   * ����InPoolThreadCount�ĵ�ǰֵ
   */
  unsigned int Remove();
  /*
   */
  template <typename TFunc_Ty, typename... Args_Ty>
  [[nodiscard]] auto AddTaskItem(TFunc_Ty &&tfunc, Args_Ty &&...args)
      -> std::shared_ptr<std::future<std::result_of_t<TFunc_Ty(Args_Ty...)>>> {
    if (!InitTag.load() || !RunningTag())
      return nullptr;
    //
    using Result_Ty = std::result_of_t<TFunc_Ty(Args_Ty...)>;
    auto taskSPtr = std::make_shared<std::packaged_task<Result_Ty()>>(
      // ����std::bind������д
        [tf = std::forward<TFunc_Ty>(tfunc),
         ... args = std::forward<Args_Ty>(args)]() mutable -> Result_Ty {
          return std::invoke(tf, std::forward<Args_Ty>(args)...);
        });
    std::future<Result_Ty> resFu = taskSPtr->get_future();
    {
      std::lock_guard<std::mutex> lk(InternalMutex);
      TaskItemQueue.emplace([taskSPtr]() { (*taskSPtr)(); });
    }
    CondVar.notify_all();
    return std::make_shared<std::future<Result_Ty>>(std::move(resFu));
  }

  private:
  unsigned int ConfirmThreadCount(unsigned int expectCount);
}; // class ZhousThreadPool
/*
 */
struct ThreadSPtrWapper {
  ThreadSPtrWapper() = default;
  ThreadSPtrWapper(const ThreadSPtrWapper &) = delete;
  ~ThreadSPtrWapper();
  ThreadSPtrWapper &operator=(const ThreadSPtrWapper &) = delete;

private:
  std::shared_ptr<std::thread> ThreadSPtr{nullptr};

}; // struct ThreadSPtrWapper

} // namespace CustomerDefined