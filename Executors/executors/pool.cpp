#include "pool.h"
#include "executors.h"
#include <cassert>
#include <iostream>

std::shared_ptr<Task> ThreadPool::GetTaskFromReadyQueue() {
    std::unique_lock<std::mutex> pool_guard(pool_mutex_);
    while (turned_on_ && ready_tasks_.empty()) {
        pool_cv_.wait(pool_guard);
    }
    if (!turned_on_ && ready_tasks_.empty()) {
        return nullptr;
    }

    auto cur_task = ready_tasks_[0];
    ready_tasks_.pop_front();
    return cur_task;
}

void ThreadPool::ProcessTask(std::shared_ptr<Task> task) const {
    std::unique_lock<std::mutex> task_guard(task->task_mutex_);
    if (task->is_canceled_) {
        return;
    }
    task_guard.unlock();

    try {
        task->run();
        task->MarkAsCompleted();
    } catch (const std::exception&) {
        task->MarkAsFailed(std::current_exception());
    }

    task->Finish();
}

std::optional<std::shared_ptr<Task>> ThreadPool::GetTaskFromTimeHeap() {
    std::unique_lock<std::mutex> pool_guard(pool_mutex_);
    std::chrono::system_clock::time_point deadline;
    while (turned_on_ && (time_heap_.empty()
        || (deadline = time_heap_.top().deadline) > std::chrono::system_clock::now())) {
        if (time_heap_.empty()) {
            time_heap_cv_.wait(pool_guard);
        } else {
            time_heap_cv_.wait_until(pool_guard, deadline);
        }
    }

    if (time_heap_.empty()) {
        return std::nullopt;
    }

    auto cur_task = time_heap_.top().task.lock();
    time_heap_.pop();
    return {cur_task};
}

void ThreadPool::PushFromTimeHeap(std::shared_ptr<Task> task) {
    std::unique_lock<std::mutex> pool_guard(pool_mutex_);
    std::unique_lock<std::mutex> task_guard(task->task_mutex_);
    if (!task->pushed_in_ready_queue_) {
        ready_tasks_.emplace_back(task);
        task_storage_.erase(task);
        task->pushed_in_ready_queue_ = true;
        pool_cv_.notify_one();
    }
}

ThreadPool::ThreadPool(int threads_num) {
    for (int ind = 0; ind < threads_num; ++ind) {
        workers_.emplace_back([this]() {
            while (true) {
                auto cur_task = GetTaskFromReadyQueue();
                if (!cur_task) {
                    return;
                }
                ProcessTask(cur_task);
            }
        });
    }

    // now create thread to manage the time_heap
    workers_.emplace_back([this]() {
        while (true) {
            auto task_opt = GetTaskFromTimeHeap();
            if (!task_opt) {
                return;
            }
            auto cur_task = task_opt.value();
            if (!cur_task) {
                continue;
            }
            PushFromTimeHeap(cur_task);
        }
    });
}

ThreadPool::~ThreadPool() {
    startShutdown();
    waitShutdown();
}

void ThreadPool::submit(std::shared_ptr<Task> task) {
    if (!task) {
        return;
    }
    std::unique_lock<std::mutex> pool_guard(pool_mutex_);
    // here we lock tasks mutex
    std::unique_lock<std::mutex> task_guard(task->task_mutex_);

    if (task->is_canceled_) {
        return;
    }

    if (!turned_on_) {
        task->is_canceled_ = true;
        task_guard.unlock();
        pool_guard.unlock();
        task->Finish();
        return;
    }

    if ((!task->has_dependencies_ && !task->has_trigger_ && !task->has_deadline_)
        || (task->has_trigger_ && task->was_triggered_)
        || (task->has_dependencies_ && task->dependencies_num_ == 0)) {
        ready_tasks_.emplace_back(task);
        task->pushed_in_ready_queue_ = true;
        pool_cv_.notify_one();
    } else {
        if (task->has_deadline_) {
            time_heap_.emplace(task, task->deadline_);
            time_heap_cv_.notify_one();
        }
        task_storage_.insert(task);
    }
    task->owner_pool_ = this;
    task->is_submitted_ = true;
}

void ThreadPool::startShutdown() {
    std::unique_lock<std::mutex> guard(pool_mutex_);
    turned_on_ = false;
    pool_cv_.notify_all();
    time_heap_cv_.notify_all();
}

void ThreadPool::waitShutdown() {
    std::unique_lock<std::mutex> guard(shutdown_mutex_);
    for (auto& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

std::shared_ptr<Executor> MakeThreadPoolExecutor(int num_threads) {
    return std::make_shared<ThreadPool>(num_threads);
}
