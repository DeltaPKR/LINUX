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

void * worker_second(void * arg)
{
    shared_array * arr = static_cast<shared_array *>(arg);
    while (!stop_flag.load())
    {
        shared_array::scoped_lock lk(*arr);
        std::size_t cnt = std::min(arr->size(), static_cast<std::size_t>(10));
        std::cout << "second:";
        for (std::size_t i = 0; i < cnt; ++i) std::cout << ' ' << (*arr)[i];
        std::cout << std::endl;
        for (std::size_t i = 0; i < arr->size(); ++i) (*arr)[i] = 0;
        std::cout << "second: zeroed" << std::endl;
        usleep(1200000);
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
    pthread_create(&th, nullptr, worker_second, &arr);

    while (!stop_flag.load()) sleep(1);

    pthread_join(th, nullptr);
    std::cout << "second: exiting" << std::endl;
    return 0;
}
