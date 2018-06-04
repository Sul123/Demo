#pragma once
#include <chrono>
#include <condition_variable>
#include <exception>
#include <memory>
#include <mutex>
#include <set>
#include <thread>
#include <vector>
#include <functional>

class ThreadPool;

class Task : public std::enable_shared_from_this<Task> {
public:
    virtual ~Task() {}

    virtual void run() = 0;

    void addDependency(std::shared_ptr<Task> dep);

    void addTrigger(std::shared_ptr<Task> dep);

    void setTimeTrigger(std::chrono::system_clock::time_point at);

private:
    void AddSlave(std::shared_ptr<Task> slave);
    void PutUnderTrigger(std::shared_ptr<Task> victim);
    void MarkAsFailed(std::exception_ptr error_ptr);
    void MarkAsCompleted();

    void PushInReadyQueue(const std::shared_ptr<Task>& task) const;

    void ReleaseDependencies();
    void ReleaseTriggers();
    void Finish();

public:
    // Task::run() completed without throwing exception
    bool isCompleted();
    // Task::run() throwed exception
    bool isFailed();
    // Task was canceled
    bool isCanceled();
    // Task either completed, failed or was canceled
    bool isFinished();

    std::exception_ptr getError();

    void cancel();

    void wait();

private:
    friend class ThreadPool;
    ThreadPool* owner_pool_ = nullptr;
    std::condition_variable task_cv_;
    std::mutex task_mutex_, process_mutex_;

    bool process_started_ = false;

    bool is_submitted_ = false;
    bool pushed_in_ready_queue_ = false;

    bool is_canceled_ = false;
    bool is_failed_ = false;
    bool is_completed_ = false;

    std::exception_ptr exception_ptr_;

    bool slaves_taken_ = false;
    bool victims_taken_ = false;

    bool has_dependencies_ = false;
    int dependencies_num_ = 0;
    bool has_trigger_ = false;
    bool was_triggered_ = false;
    bool has_deadline_ = false;
    std::chrono::system_clock::time_point deadline_;
    std::vector<std::weak_ptr<Task>> slaves_;
    std::vector<std::weak_ptr<Task>> victims_;
};

template <class T>
class Future;

template <class T>
using FuturePtr = std::shared_ptr<Future<T>>;

// Used instead of void in generic code
struct Unit {};

class Executor {
public:
    virtual ~Executor() {}

    virtual void submit(std::shared_ptr<Task> task) = 0;

    virtual void startShutdown() = 0;
    virtual void waitShutdown() = 0;

    template <class T>
    FuturePtr<T> invoke(std::function<T()> fn) {
        auto future = std::make_shared<Future<T>>(fn);
        submit(future);
        return future;
    }

    template <class Y, class T>
    FuturePtr<Y> then(FuturePtr<T> input, std::function<Y()> fn) {
        auto future = std::make_shared<Future<Y>>(fn);
        future->addDependency(input);
        submit(future);
        return future;
    }

    template <class T>
    FuturePtr<std::vector<T>> whenAll(std::vector<FuturePtr<T>> all) {
        auto future = std::make_shared<Future<std::vector<T>>>([all]() {
            std::vector<T> results;
            for (auto dep_future : all) {
                if (dep_future->isCompleted()) {
                    results.push_back(dep_future->get());
                }
            }
            return results;
        });
        for (auto cur_future : all) {
            future->addDependency(cur_future);
        }
        submit(future);
        return future;
    }

    template <class T>
    FuturePtr<T> whenFirst(std::vector<FuturePtr<T>> all) {
        auto future = std::make_shared<Future<T>>([all]() {
            for (auto cur_future : all) {
                if (cur_future->isCompleted()) {
                    return cur_future->get();
                }
            }
            throw std::runtime_error("Looks like all tasks failed");
        });
        for (auto cur_future : all) {
            future->addTrigger(cur_future);
        }
        submit(future);
        return future;
    }

    template <class T>
    FuturePtr<std::vector<T>> whenAllBeforeDeadline(std::vector<FuturePtr<T>> all,
                                                    std::chrono::system_clock::time_point deadline) {
        auto future = std::make_shared<Future<std::vector<T>>>([all]() {
            std::vector<T> results;
            for (auto cur_future : all) {
                if (cur_future->isCompleted()) {
                    results.push_back(cur_future->get());
                }
            }
            return results;
        });
        for (auto cur_future : all) {
            future->addDependency(cur_future);
        }
        future->setTimeTrigger(deadline);
        submit(future);
        return future;
    }
};

std::shared_ptr<Executor> MakeThreadPoolExecutor(int num_threads);

template <class T>
class Future : public Task {
public:
    Future() = default;

    explicit Future(const std::function<T()>& func)
        : func_(func) {}

    void run() override {
        result_ = func_();
    }

    T get() {
        wait();
        if (isFailed()) {
            std::rethrow_exception(getError());
        }
        return result_;
    }

private:
    std::function<T()> func_;
    T result_;
};
