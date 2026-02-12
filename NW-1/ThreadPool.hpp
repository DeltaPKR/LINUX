#pragma once
#include <thread>
#include <vector>
#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <functional>

namespace SimpleNet 
{

class ThreadPool 
{
public:
    ThreadPool(size_t threads);

    template<class F>
    void enqueue(F&& task) 
    {
        auto wrapper = std::make_unique<std::function<void()>>(std::forward<F>(task));
        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            tasks.push(std::move(wrapper));
        }
        cv.notify_one();
    }

    ~ThreadPool();

private:
    std::vector<std::thread> workers;
    std::queue<std::unique_ptr<std::function<void()>>> tasks;

    std::mutex queue_mutex;
    std::condition_variable cv;
    std::atomic<bool> stop_flag;
};

}

