#pragma once
#include <deque>
#include <queue>
#include <optional>
#include "executors.h"

struct TimedTask {
    std::weak_ptr<Task> task;
    std::chrono::system_clock::time_point deadline;

    explicit TimedTask(const std::shared_ptr<Task>& cur_task,
                       std::chrono::system_clock::time_point cur_deadline)
        : task(cur_task), deadline(cur_deadline) {}

    // inverse comparator to simplify priority_queue initialization
    bool operator<(const TimedTask& other) const {
        return deadline > other.deadline;
    }
};

class ThreadPool : public Executor {
public:
    explicit ThreadPool(int threads_num);
    ~ThreadPool() override;

    void submit(std::shared_ptr<Task> task) override;

    void startShutdown() override;
    void waitShutdown() override;

private:
    std::shared_ptr<Task> GetTaskFromReadyQueue();
    void ProcessTask(std::shared_ptr<Task> task) const;
    std::optional<std::shared_ptr<Task>> GetTaskFromTimeHeap();
    void PushFromTimeHeap(std::shared_ptr<Task> task);

    friend class Task;
    std::vector<std::thread> workers_;
    std::mutex pool_mutex_, shutdown_mutex_;
    std::condition_variable pool_cv_, time_heap_cv_;
    std::set<std::shared_ptr<Task>> task_storage_;
    std::deque<std::shared_ptr<Task>> ready_tasks_;
    std::priority_queue<TimedTask> time_heap_;

    bool turned_on_ = true;
};
