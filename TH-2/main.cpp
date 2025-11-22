#include "parallel_scheduler.h"
#include <iostream>
#include <unistd.h>

void work(void* data)
{
    int id = *static_cast<int*>(data);
    std::cout << "Task " << id << " started\n";
    usleep(200000);
    std::cout << "Task " << id << " finished\n";
}

int main()
{
    parallel_scheduler pool(4);

    int values[20];
    for (int i = 0; i < 20; i++)
    {
        values[i] = i;
        pool.run(work, &values[i]);
    }

    sleep(3);
    return 0;
}

