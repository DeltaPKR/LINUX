#include "parallel_scheduler.h"
#include <iostream>

parallel_scheduler::parallel_scheduler(std::size_t capacity) : stop_(false)
{
    pthread_mutex_init(&mutex_, nullptr);
    pthread_cond_init(&cond_, nullptr);

    threads_.resize(capacity);

    for(std::size_t i = 0; i < capacity; ++i)
    {
        pthread_create(&threads_[i], nullptr, worker_entry, this);
    }
}

parallel_scheduler::~parallel_scheduler()
{
    pthread_mutex_lock(&mutex_);
    stop_ = true;
    pthread_mutex_unlock(&mutex_);

    pthread_cond_broadcast(&cond_);

    for(auto& t : threads_) pthread_join(t, nullptr);

    pthread_mutex_destroy(&mutex_);
    pthread_cond_destroy(&cond_);
}

void parallel_scheduler::run(std::function<void(void*)> func, void* arg)
{
    pthread_mutex_lock(&mutex_);
    queue_.push(Task{func, arg});
    pthread_mutex_unlock(&mutex_);
    pthread_cond_signal(&cond_);
}

void* parallel_scheduler::worker_entry(void* arg)
{
    static_cast<parallel_scheduler*>(arg)->worker_loop();
    return nullptr;
}

void parallel_scheduler::worker_loop()
{
    while(true)
    {
        pthread_mutex_lock(&mutex_);

        while (!stop_ && queue_.empty()) pthread_cond_wait(&cond_, &mutex_);

        if(stop_ && queue_.empty())
        {
            pthread_mutex_unlock(&mutex_);
            return;
        }

        Task t = queue_.front();
        queue_.pop();

        pthread_mutex_unlock(&mutex_);

        t.func(t.arg);
    }
}

