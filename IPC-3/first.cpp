#include "shared_array.h"
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <algorithm>
#include <csignal>
#include <atomic>

static std::atomic<bool> stop_flag{false};

void signal_handler(int)
{
    stop_flag.store(true);
}

void * worker_first(void * arg)
{
    shared_array * arr = static_cast<shared_array *>(arg);
    while (!stop_flag.load())
    {
        shared_array::scoped_lock lk(*arr);
        for (std::size_t i = 0; i < arr->size(); ++i)
        {
            (*arr)[i] += 1;
        }
        std::size_t cnt = std::min(arr->size(), static_cast<std::size_t>(10));
        std::cout << "first:";
        for (std::size_t i = 0; i < cnt; ++i) std::cout << ' ' << (*arr)[i];
        std::cout << std::endl;
        usleep(700000);
    }
    return nullptr;
}

int main(int argc, char ** argv)
{
    std::signal(SIGINT, signal_handler);
    std::string name = (argc > 1) ? argv[1] : "array";
    std::size_t n = (argc > 2) ? static_cast<std::size_t>(std::stoul(argv[2])) : 10;
    shared_array arr(name, n);

    pthread_t th;
    pthread_create(&th, nullptr, worker_first, &arr);

    // Wait until signal sets stop_flag
    while (!stop_flag.load()) sleep(1);

    // join worker then exit
    pthread_join(th, nullptr);
    std::cout << "first: exiting" << std::endl;
    return 0;
}
