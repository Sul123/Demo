#include "executors.h"
#include <cassert>
#include "pool.h"

void Task::addDependency(std::shared_ptr<Task> dep) {
    if (!dep || dep.get() == this) {
        return;
    }
    dep->AddSlave(shared_from_this());
}

void Task::addTrigger(std::shared_ptr<Task> dep) {
    if (!dep || dep.get() == this) {
        return;
    }
    dep->PutUnderTrigger(shared_from_this());
}

void Task::setTimeTrigger(std::chrono::system_clock::time_point at) {
    std::unique_lock<std::mutex> guard(task_mutex_);
    deadline_ = at;
    has_deadline_ = true;
}

void Task::AddSlave(std::shared_ptr<Task> slave) {
    std::unique_lock<std::mutex> master_guard(task_mutex_);
    std::unique_lock<std::mutex> slave_guard(slave->task_mutex_);
    slave->has_dependencies_ = true;
    if (is_failed_ || is_canceled_ || is_completed_) {
        return;
    }
    slaves_.emplace_back(slave);
    slave->dependencies_num_ += 1;
}

void Task::PutUnderTrigger(std::shared_ptr<Task> victim) {
    std::unique_lock<std::mutex> killer_guard(task_mutex_);
    std::unique_lock<std::mutex> victim_guard(victim->task_mutex_);
    victim->has_trigger_ = true;
    if (is_failed_ || is_canceled_ || is_completed_) {
        victim->was_triggered_ = true;
    }
    victims_.emplace_back(victim);
}

void Task::PushInReadyQueue(const std::shared_ptr<Task> &task) const {
    auto task_owner_pool = task->owner_pool_;
    assert(task_owner_pool);
    std::unique_lock<std::mutex> owner_pool_guard(task_owner_pool->pool_mutex_);
    std::unique_lock<std::mutex> task_guard(task->task_mutex_);
    if (task->pushed_in_ready_queue_) {
        return;
    }
    task_owner_pool->ready_tasks_.emplace_back(task);
    task_owner_pool->task_storage_.erase(task);
    task->pushed_in_ready_queue_ = true;
    task_owner_pool->pool_cv_.notify_one();
}

void Task::ReleaseDependencies() {
    std::unique_lock<std::mutex> task_guard(task_mutex_);
    if (slaves_taken_) {
        return;
    }
    std::vector<std::weak_ptr<Task>> slaves = std::move(slaves_);
    slaves_taken_ = true;
    task_guard.unlock();

    for (auto slave : slaves) {
        if (auto slave_task = slave.lock()) {
            std::unique_lock<std::mutex> slave_task_guard(slave_task->task_mutex_);
            --slave_task->dependencies_num_;
            if (slave_task->is_submitted_ && !slave_task->pushed_in_ready_queue_
                && slave_task->dependencies_num_ == 0) {
                slave_task_guard.unlock();
                PushInReadyQueue(slave_task);
            }
        }
    }
}

void Task::ReleaseTriggers() {
    std::unique_lock<std::mutex> task_guard(task_mutex_);
    if (victims_taken_) {
        return;
    }
    std::vector<std::weak_ptr<Task>> victims = std::move(victims_);
    victims_taken_ = true;
    task_guard.unlock();

    for (auto victim : victims) {
        if (auto victim_task = victim.lock()) {
            std::unique_lock<std::mutex> victim_task_guard(victim_task->task_mutex_);
            victim_task->was_triggered_ = true;
            if (victim_task->is_submitted_ && !victim_task->pushed_in_ready_queue_) {
                victim_task_guard.unlock();
                PushInReadyQueue(victim_task);
            }
        }
    }
}

void Task::Finish() {
    ReleaseDependencies();
    ReleaseTriggers();
    task_cv_.notify_all();
}

bool Task::isCompleted() {
    std::unique_lock<std::mutex> guard(task_mutex_);
    return is_completed_;
}

// Task::run() throwed exception
bool Task::isFailed() {
    std::unique_lock<std::mutex> guard(task_mutex_);
    return is_failed_;
}

// Task was canceled
bool Task::isCanceled() {
    std::unique_lock<std::mutex> guard(task_mutex_);
    return is_canceled_;
}

// Task either completed, failed or was canceled
bool Task::isFinished() {
    std::unique_lock<std::mutex> guard(task_mutex_);
    return is_failed_ || is_canceled_ || is_completed_;
}

std::exception_ptr Task::getError() {
    std::unique_lock<std::mutex> guard(task_mutex_);
    return exception_ptr_;
}

void Task::MarkAsFailed(std::exception_ptr error_ptr) {
    std::unique_lock<std::mutex> guard(task_mutex_);
    is_failed_ = true;
    exception_ptr_ = error_ptr;
}

void Task::cancel() {
    std::unique_lock<std::mutex> guard(task_mutex_);
    is_canceled_ = true;
    guard.unlock();
    Finish();
}

void Task::MarkAsCompleted() {
    std::unique_lock<std::mutex> guard(task_mutex_);
    is_completed_ = true;
}

void Task::wait() {
    std::unique_lock<std::mutex> guard(task_mutex_);
    while (!is_failed_ && !is_canceled_ && !is_completed_) {
        task_cv_.wait(guard);
    }
}
