#ifndef PARALLEL_SCHEDULER_H
#define PARALLEL_SCHEDULER_H

#include <pthread.h>
#include <queue>
#include <vector>
#include <functional>

class parallel_scheduler
{
public:
    parallel_scheduler(std::size_t capacity);
    ~parallel_scheduler();

    void run(std::function<void(void*)> func, void* arg);

private:
    struct Task
    {
        std::function<void(void*)> func;
        void* arg;
    };

    static void* worker_entry(void* arg);
    void worker_loop();

    std::queue<Task> queue_;
    std::vector<pthread_t> threads_;

    pthread_mutex_t mutex_;
    pthread_cond_t cond_;

    bool stop_;
};

#endif

