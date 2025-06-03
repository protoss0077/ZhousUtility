#pragma once
//
#include "ThreadWapper.h"
//
#include <chrono>
//
#include <queue>
#include <unordered_set>
//
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
  using TaskItem_Ty = std::function<void()>;
  //
  static constexpr const unsigned int InfiniteRepeated{-1};

private:
  /*struct PriorityTaskItem
   * ���ȼ�������ά������
   * ��ʱ���ΪȨ������������
   * �ڲ�ʹ��
   */
  struct PriorityTaskItem {
    using OriginTaskItem_Ty = TaskItem_Ty;
    //
    TimePoint_Ty StartTp;
    //
    OriginTaskItem_Ty TaskItem;
    //
    RepeatedTaskId_Ty RepeatedId;
    //
    PriorityTaskItem() = delete;
    //
    PriorityTaskItem(const PriorityTaskItem &) = default;
    //
    ~PriorityTaskItem() {}
    //
    PriorityTaskItem &operator=(const PriorityTaskItem &) = default;
    //
    bool operator<(const PriorityTaskItem &other) const {
      return StartTp > other.StartTp;
    }
  }; // struct PriorityTaskItem

  /* PriorityTimer
   * ά���ڲ��̺߳�������У��Է���ʱ���ΪȨ�����ȼ����У�
   */
  class PriorityTimer {
  private:
    std::priority_queue<PriorityTaskItem> TaskItemPQueue;
    //
    std::unordered_set<RepeatedTaskId_Ty> RepeatedTaskIdUSet;
    //
    std::unique_ptr<ThreadWapper> InternalThreadUPtr;
    //
    std::mutex InternalMutex;
    std::condition_variable CondVar;
    //
    std::atomic<bool> RunningTag;
    //
    std::atomic<RepeatedTaskId_Ty> NextRepeatedTaskId;

  public:
    PriorityTimer();
    //
    PriorityTimer(const PriorityTimer &) = delete;
    //
    ~PriorityTimer();
    //
    PriorityTimer &operator=(const PriorityTimer &) = delete;
    /* Run
     * ���RunningTagΪtrue�����޲���
     * ���RunningTagΪfalse������RunningTag
     * �������ڲ�����
     */
    void Start();
    /* ReStart;
     * ���RunningTagΪtrue�����޲���
     * ���RunningTagΪfalse������RunningTag
     * �� �������ڲ�����
     * �����ͣ�ڼ�ѻ������������Ҫ�����ж��Ƿ��������
     */
    void ReStart();
    /* Pause
     * �����߳� һ��ʱ��
     * �� �������ڲ�����
     * �����ͣ�ڼ�ѻ������������Ҫ�����ж��Ƿ��������
     */
    void Pause();
    void Pause(const std::chrono::microseconds&timeoffMicroSec);
    /* Stop
    * ����RunningTagΪfalse
    * ����ڲ�����
    */
    void Stop();
    /* AddRealTimeTask
     * ���һ��������������ȼ���ִ�е����������
     */
    void AddRealTimeTask(TaskItem_Ty titem);
    /* AddNormalTask
     * ��ӳ�������
     * �����ָ�����ں��ظ�����Ĭ���ظ�1��
     * InfiniteRepeated�ظ�ֱ��ȡ��Ϊֹ
     */
    RepeatedTaskId_Ty AddNormalTask(
        TaskItem_Ty titem, const std::chrono::microseconds &delayedMicroSec,
        const std::chrono::microseconds &periodMicroSec, unsigned int repCount);
    RepeatedTaskId_Ty
    AddNormalTask(TaskItem_Ty titem,
                  const std::chrono::microseconds &delayedMicroSec);
    /* TryCancelRepeatedTask
     * ����ɾ��һ������
     * �ѿ�ʼ�������޷�ȡ��
     */
    bool TryCancelSpecifiedTask(RepeatedTaskId_Ty rtaskId);
    /* DeleteAllTasts
     * ����ڲ�����
     */
    void DeleteAllTasts();

  private:
    void Run_();
    //
    void DefThreadFunc();

  }; // class PriorityTimer

}; // class SlimSerialExecutor
} // namespace CustomerDefined