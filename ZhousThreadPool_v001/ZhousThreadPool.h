#pragma once
//
#include <string>
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
  // ����ʱ�ӳ����Ƴ��̵߳ĳ�ʱ��(milliseconds)
  static constexpr const unsigned int AutoRemoveTimeOut{1000};

private:
  // �Ƴ���ǵĿ���ֵ
  static constexpr const unsigned int RemoveStateNone{0};
  static constexpr const unsigned int RemoveStateLastIdle{1};
  static constexpr const unsigned int RemoveStateLastEnded{2};

public:
  /* struct PoolSnapshot
   * ��Ϣ����
   */
  struct PoolSnapshot {
    bool RunningTag;
    unsigned int WorkingThreadCount;
    unsigned int InPoolThreadCount;
    unsigned int ThreadCountLimit;
    size_t TaskItemInQueue;
    //
    PoolSnapshot() = delete;
    PoolSnapshot(const PoolSnapshot &) = default;
    PoolSnapshot(bool rtag, unsigned int wtc, unsigned int itc,
                 unsigned int tcl, size_t tiiq);
    //
    ~PoolSnapshot() = default;
    //
    PoolSnapshot &operator=(const PoolSnapshot &) = default;
    /* ToString
     * tabViewTagΪtrue,ThreadCountLimit<=9ʱ�����б���ͼ�ַ���
     * ����:
     * Running:|0 * !1 _ 2 x 3 x 4|| 3 |
     * ��ʾ�̳߳�������
     * ������С�߳���Ϊ1������߳���Ϊ4����ǰ�����߳���Ϊ2�������߳���Ϊ1�������߳���Ϊ1
     * ����������ڲ�Ϊ�գ���3��ȴ�����
     */
    std::string ToString(bool tabViewTag) const;
    //
  private:
    std::string ToTableString() const;
    //
    std::string ToDefString() const;
  };

private:
  // Ҳ�����ͨ��һ���������������棿
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
  // ���б�� true/false
  std::atomic<bool> RunningTag;
  // �Ƴ����
  std::atomic<unsigned int> RemoveState;

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
   * ����RunningTag�����0���߳�
   * ���RunningTag�����ò���ִ�в���
   */
  void Start();
  //
  bool IsRunning() const { return RunningTag.load(); }
  //
  void Stop();
  /* TryAddThread
   * ��������ʱ����Ϊ�̳߳����һ�������߳�
   * ������
   * InPoolThreadCountС��MaxThreadCountLimit
   * �Ҷ��в�Ϊ����û�������߳�
   * ����InPoolThreadCount�ĵ�ǰֵ
   */
  unsigned int TryAddThread();
  /* GetSnapshot
   * ��ȡ����
   */
  PoolSnapshot GetSnapshot() const;
  /*
   */
  template <typename TFunc_Ty, typename... Args_Ty>
  void AddNoReturnTaskItem(TFunc_Ty &&tfunc, Args_Ty &&...args) {
    if (!RunningTag.load())
      throw std::runtime_error("�̳߳�δ����...");
    auto titem = std::bind(std::forward<TFunc_Ty>(tfunc),
                           std::forward<Args_Ty>(args)...);
    {
      std::lock_guard<std::mutex> lk(InternalMutex);
      TaskItemQueue.emplace([titem]() { titem(); });
    }
    CondVar.notify_all();
  }
  /* AddTaskItem
   * ��ӹ�����
   * ȥ�˵�shared_ptr��Ƕ��
   */
  template <typename TFunc_Ty, typename... Args_Ty>
  [[nodiscard]] auto AddTaskItem(TFunc_Ty &&tfunc, Args_Ty &&...args)
      -> std::future<std::result_of_t<TFunc_Ty(Args_Ty...)>> {
    if (!RunningTag.load())
      throw std::runtime_error("�̳߳�δ����...");
    //
    using Result_Ty = std::result_of_t<TFunc_Ty(Args_Ty...)>;
    // std::bind������д
    // ��lambda���ڸ��ӣ�...args�Ĳ�����ҪC++20?
    /*
    auto taskItem = std::make_shared<std::packaged_task<Result_Ty()>>(
        [tf = std::forward<TFunc_Ty>(tfunc),
         ... args = std::forward<Args_Ty>(args)]() mutable -> Result_Ty {
          return std::invoke(tf, std::forward<Args_Ty>(args)...);
        });
    */
    // ���������ָ�룡����
    auto taskItemSPtr =
        std::make_shared<std::packaged_task<Result_Ty()>>(std::bind(
            std::forward<TFunc_Ty>(tfunc), std::forward<Args_Ty>(args)...));
    std::future<Result_Ty> resFu = taskItemSPtr->get_future();
    {
      std::lock_guard<std::mutex> lk(InternalMutex);
      TaskItemQueue.emplace([taskItemSPtr]() { (*taskItemSPtr)(); });
    }
    CondVar.notify_all();
    return resFu;
  }

private:
  /* ConfirmThreadCount
   * ȷ�����յ�ThreadCountLimit
   */
  static unsigned int ConfirmThreadCount(unsigned int expectCount);
  /* DefThreadFunc
   * �����̺߳���
   * �����߳���Thread0���/�Ƴ�
   * ����ֻ�ǽ�����������
   */
  void DefThreadFunc();
  /* ThreadFunc0
   * 0���̺߳���
   */
  void Thread0Func();

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
    if (InternalThread.joinable())
      InternalThread.join();
  }
  //
  std::thread::id GetId() { return InternalThread.get_id(); }

private:
  std::thread InternalThread;
}; // struct ThreadWapper

} // namespace CustomerDefined