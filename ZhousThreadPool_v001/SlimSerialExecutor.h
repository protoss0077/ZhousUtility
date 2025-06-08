#pragma once
//
#include "ThreadWapper.h"
//
#include <chrono>
//
#include <queue>
#include <unordered_set>
//
#include <future>
#include <mutex>
//
namespace CustomerDefined {
class SlimSerialExecutor {
public:
  using TaskRunnerTag_Ty = uint64_t;
  using RepeatedTaskId_Ty = uint64_t;
  // ִ���߳�
  using TaskRunner_Ty = ThreadWapper;
  using TaskRunnerUPtr_Ty = std::unique_ptr<TaskRunner_Ty>;
  //
  using Clock_Ty = std::chrono::high_resolution_clock;
  using TimePoint_Ty = std::chrono::time_point<Clock_Ty>;
  using Duration_Ty = std::chrono::microseconds;
  //
  using TaskItem_Ty = std::function<void()>;
  //
  static constexpr const uint64_t InfiniteRepeated{-1};
  static constexpr const uint64_t InvalidRepeatedTaskId{-1};

private:
  // PriorityTimerImpl ǰ������
  // �ڲ�ʵ��
  class PriorityTimerImpl;
  /* PriorityTimer
   * ά���ڲ��̺߳�������У��Է���ʱ���ΪȨ�����ȼ����У�
   */
  class PriorityTimer {
  private:
    std::unique_ptr<PriorityTimerImpl> PTimerUPtr;

  public:
    PriorityTimer();
    //
    PriorityTimer(const PriorityTimer &) = delete;
    //
    ~PriorityTimer();
    //
    PriorityTimer &operator=(const PriorityTimer &) = delete;
    /* Start
     * ���RunningTagΪtrue�����޲���
     * ���RunningTagΪfalse������RunningTag
     * �������ڲ�����
     */
    void Start();
    /* Stop
     * ����RunningTagΪfalse
     * ����ڲ�����
     */
    void Stop();
    /* Resume
     * �˽ӿ� �� �������ڲ�����
     * �����ͣ�ڼ�ѻ������������Ҫ�����ж��Ƿ��������
     */
    void Resume();
    /* Pause
     * �����߳� һ��ʱ��
     * ָ��ʱ����ʵ�����һ��ʵʱ����
     */
    void Pause();
    void Pause(const std::chrono::microseconds &timeoffMicroSec);
    /* Addϵ��
     * ��ӳ��棨�޲���������
     * �����ָ�����ں��ظ�����Ĭ���ظ�1�Σ��ӿ�1��
     * ���᷵��RepeatedTaskId�����Բ����¼Ϊ�ظ�����Ҳ����ȡ��
     * ���ӿ�2�����Ը�����Ҫ����RepeatedTaskId
     * ���Գ���ȡ�����ظ����񣬵��Ѿ���ӽ������е����񲻻�ȡ��
     * ΪrepCountָ�� InfiniteRepeated �������ظ�ֱ��ȡ��Ϊֹ
     */
    void AddRealTimeTask(TaskItem_Ty &&titem);
    void AddNormalTask(TaskItem_Ty &&titem, Duration_Ty delayMicroSec);
    //
    [[nodiscard]] RepeatedTaskId_Ty AddNormalTask(
        TaskItem_Ty &&titem, const std::chrono::microseconds &delayMicroSec,
        const std::chrono::microseconds &periodMicroSec, uint64_t repCount);
    /* TryCancelSpecifiedTask
     * ����ɾ��һ������ӵ�NormalTask,
     * rtaskIdͨ��AddNormalTask���
     * �ѽ�����е������޷�ȡ��
     */
    bool TryCancelSpecifiedTask(RepeatedTaskId_Ty rtaskId);
    /* DeleteAllTasts
     * ����ڲ�����
     */
    void DeleteAllTasks();
    /* Postϵ��
     * ֻ�ṩ��Ҫ�����з���ֵ�����ת��
     * ���ṩ�ظ�����(��װ̫�鷳)
     * ���÷���ֵʱ�ɵ����߷�װ
     */
    template <typename Func_Ty, typename... Args_Ty>
    auto PostRealTimeTask(Func_Ty &&func, Args_Ty... args)
        -> std::future<std::result_of_t<Func_Ty(Args_Ty...)>> {
      using Result_Ty = std::result_of_t<Func_Ty(Args_Ty...)>;
      auto titemSPtr = std::make_shared<std::packaged_task<Result_Ty>>(
          std::bind(std::forward<Func_Ty>(std::move(func)),
                    std::forward<Args_Ty>(args)...));
      Result_Ty resFu = titemSPtr->get_future();
      AddRealTimeTask([titemSPtr]() { (*titemSPtr)(); })
    }
    //
    template <typename Du_Ty, typename Func_Ty, typename... Args_Ty>
    auto PostNormalTask(Du_Ty delay, Func_Ty &&func, Args_Ty &&...args)
        -> std::future<std::result_of_t<Func_Ty(Args_Ty...)>> {
      using Result_Ty = std::result_of_t<Func_Ty(Args_Ty...)>;
      auto titemSPtr = std::make_shared<std::packaged_task<Result_Ty>>(
          std::bind(std::forward<Func_Ty>(std::move(func)),
                    std::forward<Args_Ty>(args)...));
      Result_Ty resFu = titemSPtr->get_future();
      AddRealTimeTask([titemSPtr, delay]() { (*titemSPtr)(); },
                      std::chrono::duration_cast<Duration_Ty>(delay));
    }
  }; // class PriorityTimer
private:
  PriorityTimer PTimer;
}; // class SlimSerialExecutor
} // namespace CustomerDefined