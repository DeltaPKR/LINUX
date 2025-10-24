#include <iostream>
#include <vector>
#include <pthread.h>
#include <random>
#include <algorithm>
#include <chrono>
#include <cstring>

class Data
{
public:
    pthread_t tid;
    int* arr;
    int start;
    int end;
    double result;
};

void* run(void* arg)
{
    Data* td = static_cast<Data*>(arg);
    
    int lenght = td->end - td->start;
    if(lenght == 0) 
    {
        td->result = 0.0;
        return arg;
    }
    double s = 0.0;
    
    for(int i = td->start; i < td->end; ++i)
    {
        s += td->arr[i];
    }
    td->result = s;
    return arg;
}

int main(int argc, char** argv)
{
    if(argc < 3)
    {
        std::cerr << "usage: " << argv[0] << " N M" << std::endl;
        return 1;
    }
    
    unsigned long long N = std::stoull(argv[1]);
    unsigned long long M = std::stoull(argv[2]);
    
    if(N < 1 || M < 1)
    {
        std::cerr << "N and M must be >= 1" << std::endl;
        return 1;
    }
    
    if(M > N) M = N;
    
    std::vector<int> nums(N);
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis (0, 100);
    std::generate(nums.begin(), nums.end(), [&]() {return dis(gen);} );
    
    double single_sum = 0.0;
    auto t1 = std::chrono::high_resolution_clock::now();
    for (unsigned long long i = 0; i < N; ++i)
    {
        single_sum += nums[i];
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration< double, std::milli > single_ms = t2 - t1;
    
    std::vector<Data>th_datas(M);
    int chunk = nums.size() / M;
    int offset = 0;
    for(unsigned long long i = 0; i <  M; ++i)
    {
        th_datas[i].arr = nums.data();
        th_datas[i].start = offset;
        if(i+1 == M)
        {
            th_datas[i].end = nums.size();
        }
        else
        {
            th_datas[i].end = offset + chunk;
        }
        th_datas[i].result = 0.0;
        offset = th_datas[i].end;
    }
    
    auto tm1 = std::chrono::high_resolution_clock::now();
    int created = 0;
    for (unsigned long long i = 0; i < M; ++i)
    {
	int rc = pthread_create(&th_datas[i].tid, nullptr, run, &th_datas[i]);
	if(rc != 0)
	{
		std::cerr << "thread create failed at: " << i << " rc" << " (" << std::strerror(rc) << ")"  << std::endl;
		for(int j = 0; j < created; ++j)
		{
		    int jr = pthread_join(th_datas[j].tid, nullptr);
		    if(jr != 0)
		    {
			std::cerr << "pthread_join failed for" << j << " jr" << jr << " (" << std::strerror(jr) << ")" << std::endl;
		    }
		    return 1;
		}
	}
	++created;
    }
    for (int i = 0; i < created; ++i)
    {
	int jr = pthread_join(th_datas[i].tid, nullptr);
	if(jr != 0)
	{
	    std::cerr << "thread join failed" << i << " jr" << jr << " (" << std::strerror(jr) << ")" << std::endl;
	    return 1;
	}
    }

    auto tm2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> multi_ms = tm2 - tm1;

    double multi_sum = 0.0;
    for (unsigned long long i = 0; i < M; ++i)
    {
        multi_sum += th_datas[i].result;
    }
    
    std::cout << "Time spent without threads: " << single_ms.count() << " ms" << std::endl;
    std::cout << "Time spent with " << M << " threads: " << multi_ms.count() << " ms" << std::endl;
    
    return 0;
}



