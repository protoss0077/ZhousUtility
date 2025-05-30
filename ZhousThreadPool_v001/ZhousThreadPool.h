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
#include <assert.h>
//
namespace CustomerDefined {
// ǰ������
// thread�İ�װ��
// ���������join
struct ThreadWapper;
using ThreadWapperSPtr_Ty = std::shared_ptr<ThreadWapper>;
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
  // ����ʱ�ӳ����Ƴ��̵߳ĳ�ʱ��(����)
  static constexpr const unsigned int AutoRemoveTimeOut{1};

private:
  std::vector<ThreadWapperSPtr_Ty> ThreadSPtrColl;
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
  // std::atomic<bool> InitTag;
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
  /* Start
   * ����RunningTag����ӵ�1���߳�
   * ���RunningTag�����ò���ִ�в���
   */
  void Start();
  //
  bool IsRunning() { return RunningTag.load(); }
  //
  void Stop();
  /* TryAddThread
   * ��������ʱΪ�̳߳����һ�������߳�
   * ������
   * InPoolThreadCountС��MaxThreadCountLimit
   * �Ҷ��в�Ϊ��
   * ��WorkingThreadCount == InPoolThreadCount
   * ����InPoolThreadCount�ĵ�ǰֵ
   */
  unsigned int TryAddThread();
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
    if (!RunningTag.load())
      return nullptr;
    //
    using Result_Ty = std::result_of_t<TFunc_Ty(Args_Ty...)>;
    // std::bind������д
    // ��lambda���ڸ��ӣ�...args�Ĳ�����ҪC++20?
    /*
    auto taskSPtr = std::make_shared<std::packaged_task<Result_Ty()>>(
        [tf = std::forward<TFunc_Ty>(tfunc),
         ... args = std::forward<Args_Ty>(args)]() mutable -> Result_Ty {
          return std::invoke(tf, std::forward<Args_Ty>(args)...);
        });
        */
    auto taskSPtr = std::make_shared<std::packaged_task<Result_Ty()>>(std::bind(
        std::forward<TFunc_Ty>(tfunc), std::forward<Args_Ty>(args)...));
    std::future<Result_Ty> resFu = taskSPtr->get_future();
    {
      std::lock_guard<std::mutex> lk(InternalMutex);
      TaskItemQueue.emplace([taskSPtr]() { (*taskSPtr)(); });
    }
    CondVar.notify_all();
    return std::make_shared<std::future<Result_Ty>>(std::move(resFu));
  }

private:
  /* ConfirmThreadCount
   * ȷ�����յ�ThreadCountLimit
   */
  static unsigned int ConfirmThreadCount(unsigned int expectCount);
  /* DefThreadFunc
   * �̺߳���
   */
  void DefThreadFunc();

}; // class ZhousThreadPool
/*
 */
struct ThreadWapper {
  ThreadWapper() = delete;
  ThreadWapper(const ThreadWapper &) = delete;
  // ThreadWapper(ThreadWapper&&)=default;
  //
  explicit ThreadWapper(std::function<void(void)> tfunc)
      : InternalThread{std::move(tfunc)} {}
  //
  ~ThreadWapper() { Join(); }
  ThreadWapper &operator=(const ThreadWapper &) = delete;
  //
  bool Joinable() { return InternalThread.joinable(); }
  //
  void Join() {
    if (Joinable())
      InternalThread.join();
  }
  //
  std::thread::id GetId() { return InternalThread.get_id(); }

private:
  std::thread InternalThread;

}; // struct ThreadWapper

} // namespace CustomerDefined