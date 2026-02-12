#include "ThreadPool.hpp"

namespace SimpleNet 
{

ThreadPool::ThreadPool(size_t threads) : stop_flag(false) 
{
    for (size_t i = 0; i < threads; ++i) 
    {
        workers.emplace_back(
	    [this] 
	    {
                while (true) 
		{
                    std::unique_ptr<std::function<void()>> task;

                    {
                        std::unique_lock<std::mutex> lock(queue_mutex);
                        cv.wait(lock, [this] {
                        return stop_flag || !tasks.empty();
                    });

                    if (stop_flag && tasks.empty()) return;

                    task = std::move(tasks.front());
                    tasks.pop();
                }
                (*task)();
            }
        });
    }
}

ThreadPool::~ThreadPool() 
{
    {
        std::lock_guard<std::mutex> lock(queue_mutex);
        stop_flag = true;
    }

    cv.notify_all();

    for (auto& worker : workers) 
    {
        if (worker.joinable()) worker.join();
    }
}

}

