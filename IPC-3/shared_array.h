#ifndef SHARED_ARRAY_H
#define SHARED_ARRAY_H

#include <cstddef>
#include <string>
#include <semaphore.h>

class shared_array
{
public:
    shared_array(const std::string & name, std::size_t size);
    ~shared_array();

    int & operator [] (std::size_t idx);
    const int & operator [] (std::size_t idx) const;
    std::size_t size() const;

    void lock();
    void unlock();

    struct scoped_lock
    {
        scoped_lock(shared_array & arr);
        ~scoped_lock();
    private:
        shared_array & arr_;
    };

    void unlink_resources();

private:
    std::string shm_name_;
    std::string sem_name_;
    std::size_t size_ = 0;
    int shm_fd_ = -1;
    void * mapping_ = nullptr;
    sem_t * sem_ = nullptr;

    void ensure_names(const std::string & name);
};

#endif
