#include "shared_array.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdexcept>
#include <cstring>
#include <string>
#include <cerrno>

shared_array::shared_array(const std::string & name, std::size_t size)
{
    if (size == 0)
        throw std::invalid_argument("size must be > 0");

    size_ = size;
    ensure_names(name);

    std::size_t bytes = size_ * sizeof(int);

    shm_fd_ = shm_open(shm_name_.c_str(), O_RDWR | O_CREAT, 0666);
    if (shm_fd_ == -1)
    {
        throw std::runtime_error(std::string("shm_open failed: ") + std::strerror(errno));
    }

    struct stat st;
    if (fstat(shm_fd_, &st) == -1)
    {
        close(shm_fd_);
        throw std::runtime_error(std::string("fstat failed: ") + std::strerror(errno));
    }

    off_t current_size = st.st_size;

    if (current_size == 0)
    {
        if (ftruncate(shm_fd_, static_cast<off_t>(bytes)) == -1)
        {
            close(shm_fd_);
            throw std::runtime_error(std::string("ftruncate failed: ") + std::strerror(errno));
        }
    }
    else
    {
        if (static_cast<off_t>(bytes) != current_size)
        {
            close(shm_fd_);
            throw std::runtime_error("existing shared memory size differs from requested size");
        }
    }

    mapping_ = mmap(nullptr, bytes, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd_, 0);
    if (mapping_ == MAP_FAILED)
    {
        close(shm_fd_);
        throw std::runtime_error(std::string("mmap failed: ") + std::strerror(errno));
    }

    sem_ = sem_open(sem_name_.c_str(), O_CREAT, 0666, 1);
    if (sem_ == SEM_FAILED)
    {
        munmap(mapping_, bytes);
        close(shm_fd_);
        throw std::runtime_error(std::string("sem_open failed: ") + std::strerror(errno));
    }
}

shared_array::~shared_array()
{
    std::size_t bytes = size_ * sizeof(int);
    if (mapping_ && mapping_ != MAP_FAILED)
    {
        munmap(mapping_, bytes);
    }
    if (shm_fd_ != -1)
    {
        close(shm_fd_);
    }
    if (sem_ && sem_ != SEM_FAILED)
    {
        sem_close(sem_);
    }
}

int & shared_array::operator [] (std::size_t idx)
{
    if (idx >= size_) throw std::out_of_range("index out of range");
    return static_cast<int *>(mapping_)[idx];
}

const int & shared_array::operator [] (std::size_t idx) const
{
    if (idx >= size_) throw std::out_of_range("index out of range");
    return static_cast<int *>(mapping_)[idx];
}

std::size_t shared_array::size() const
{
    return size_;
}

void shared_array::lock()
{
    if (sem_ == nullptr || sem_ == SEM_FAILED) throw std::runtime_error("invalid semaphore");
    while (sem_wait(sem_) == -1)
    {
        if (errno == EINTR) continue;
        throw std::runtime_error(std::string("sem_wait failed: ") + std::strerror(errno));
    }
}

void shared_array::unlock()
{
    if (sem_ == nullptr || sem_ == SEM_FAILED) throw std::runtime_error("invalid semaphore");
    if (sem_post(sem_) == -1)
    {
        throw std::runtime_error(std::string("sem_post failed: ") + std::strerror(errno));
    }
}

shared_array::scoped_lock::scoped_lock(shared_array & arr) : arr_(arr)
{
    arr_.lock();
}

shared_array::scoped_lock::~scoped_lock()
{
    arr_.unlock();
}

void shared_array::unlink_resources()
{
    if (!sem_name_.empty())
    {
        sem_unlink(sem_name_.c_str());
    }
    if (!shm_name_.empty())
    {
        shm_unlink(shm_name_.c_str());
    }
}

void shared_array::ensure_names(const std::string & name)
{
    shm_name_ = name;
    if (shm_name_.empty()) shm_name_ = "/shared_array";
    if (shm_name_[0] != '/') shm_name_ = std::string("/") + shm_name_;

    sem_name_ = std::string("/sem_") + (shm_name_.substr(1));
    if (sem_name_[0] != '/') sem_name_ = std::string("/") + sem_name_;
}

