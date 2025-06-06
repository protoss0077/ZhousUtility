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
  static constexpr const uint64_t InfiniteRepeated{-1};
  static constexpr const uint64_t InvalidRepeatedTaskId{-1};

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
    PriorityTaskItem(const TimePoint_Ty stp, OriginTaskItem_Ty &&otitem,
                     const RepeatedTaskId_Ty rtId = InvalidRepeatedTaskId)
        : StartTp{stp}, TaskItem{std::move(otitem)}, RepeatedId{rtId} {}
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
    //
    std::priority_queue<PriorityTaskItem> TaskItemPQueue;
    // �洢�ظ�������,����ظ�����ķ�ʽ��
    // �ڶ��������һ��������ù���������������һ��ִ��repnum-1������
    std::unordered_set<RepeatedTaskId_Ty> RepeatedTaskIdUSet;
    //
    std::unique_ptr<ThreadWapper> InternalThreadUPtr;
    //
    std::mutex InternalMutex;
    std::condition_variable CondVar;
    // true-running;false-stopped
    std::atomic<bool> RunningTag;
    // true-Paused;false-running;
    std::atomic<bool> PausedTag;
    // �ظ������Ǽ���
    std::atomic<RepeatedTaskId_Ty> RepeatedTaskId;

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
    /* ReStart;
     * RunningTag-true,PausedTag-true
     * ����PausedTagΪfalse���������ߵ��߳�
     * ����������޲���
     * �˽ӿ� �� �������ڲ�����
     * �����ͣ�ڼ�ѻ������������Ҫ�����ж��Ƿ��������
     */
    void ReStart();
    /* Pause
     * �����߳� һ��ʱ��
     * ָ��ʱ����ʵ�����һ��ʵʱ����
     */
    void Pause();
    void Pause(const std::chrono::microseconds &timeoffMicroSec);
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
     * ����RepeatedTaskIdUSet
     * InfiniteRepeated�ظ�ֱ��ȡ��Ϊֹ
     */
    void AddNormalTask(TaskItem_Ty titem,
                       const std::chrono::microseconds &delayedMicroSec);
    [[nodiscard]] uint64_t AddNormalTask(
        TaskItem_Ty titem, const std::chrono::microseconds &delayedMicroSec,
        const std::chrono::microseconds &periodMicroSec, uint64_t repCount);

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
    void DefThreadFunc();
    //
    void StayAWhile(const std::chrono::microseconds &timeoffMicroSec);
    //
    RepeatedTaskId_Ty GetNextRepeatedTaskId();
    //
    void InternalAddNormalTask(TaskItem_Ty &&titem, RepeatedTaskId_Ty rtid,
                               const std::chrono::microseconds &delayedMicroSec,
                               const std::chrono::microseconds &periodMicroSec,
                               uint64_t repCount);
    // �˽ӿ�����������ظ��������ת��������ʱ
    void TaskExecuteAndAddNext(TaskItem_Ty &&titem, RepeatedTaskId_Ty rtid,
                               const std::chrono::microseconds &periodMicroSec,
                               uint64_t repCount);

  }; // class PriorityTimer

}; // class SlimSerialExecutor
} // namespace CustomerDefined