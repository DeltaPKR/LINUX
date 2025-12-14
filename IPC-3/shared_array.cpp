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
#include <cstdint>

static constexpr std::size_t MAX_ELEMENTS = 1000000000ULL; // 1e9

static std::string make_shm_name(const std::string & name)
{
    if (name.empty()) return std::string("/shared_array");
    if (name[0] != '/') return std::string("/") + name;
    return name;
}

static std::string make_sem_name(const std::string & shm_name)
{
    std::string base = (shm_name.size() > 1) ? shm_name.substr(1) : std::string("shared_array");
    return std::string("/sem_") + base;
}

shared_array::shared_array(const std::string & name, std::size_t size)
{
    if (size < 1 || size > MAX_ELEMENTS)
        throw std::invalid_argument("size must be between 1 and 1000000000");

    size_ = size;
    ensure_names(name);

    uint64_t bytes64 = static_cast<uint64_t>(size_) * static_cast<uint64_t>(sizeof(int));
    off_t bytes = static_cast<off_t>(bytes64);

    shm_fd_ = shm_open(shm_name_.c_str(), O_RDWR | O_CREAT, 0666);
    if (shm_fd_ == -1)
    {
        throw std::runtime_error(std::string("shm_open failed: ") + std::strerror(errno));
    }

    struct stat st;
    if (fstat(shm_fd_, &st) == -1)
    {
        close(shm_fd_);
        shm_unlink(shm_name_.c_str());
        throw std::runtime_error(std::string("fstat failed: ") + std::strerror(errno));
    }

    off_t current_size = st.st_size;

    if (current_size == 0)
    {
        if (ftruncate(shm_fd_, bytes) == -1)
        {
            close(shm_fd_);
            shm_unlink(shm_name_.c_str());
            throw std::runtime_error(std::string("ftruncate failed: ") + std::strerror(errno));
        }
    }
    else
    {
        if (static_cast<uint64_t>(current_size) != bytes64)
        {
            close(shm_fd_);
            throw std::runtime_error("existing shared memory size differs from requested size");
        }
    }

    mapping_ = mmap(nullptr, bytes, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd_, 0);
    if (mapping_ == MAP_FAILED)
    {
        close(shm_fd_);
        shm_unlink(shm_name_.c_str());
        throw std::runtime_error(std::string("mmap failed: ") + std::strerror(errno));
    }

    sem_ = sem_open(sem_name_.c_str(), O_CREAT, 0666, 1);
    if (sem_ == SEM_FAILED)
    {
        munmap(mapping_, bytes);
        close(shm_fd_);
        shm_unlink(shm_name_.c_str());
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

shared_array::shared_array(shared_array && other) noexcept
    : shm_name_(std::move(other.shm_name_))
    , sem_name_(std::move(other.sem_name_))
    , size_(other.size_)
    , shm_fd_(other.shm_fd_)
    , mapping_(other.mapping_)
    , sem_(other.sem_)
{
    other.size_ = 0;
    other.shm_fd_ = -1;
    other.mapping_ = nullptr;
    other.sem_ = nullptr;
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

bool shared_array::remove_shared_by_name(const std::string & name) noexcept
{
    try
    {
        std::string sname = make_shm_name(name);
        std::string semname = make_sem_name(sname);
        int r1 = shm_unlink(sname.c_str());
        int r2 = sem_unlink(semname.c_str());
        return (r1 == 0 || errno == ENOENT) && (r2 == 0 || errno == ENOENT);
    }
    catch (...) { return false; }
}

void shared_array::ensure_names(const std::string & name)
{
    shm_name_ = make_shm_name(name);
    sem_name_ = make_sem_name(shm_name_);
}

void shared_array::sync()
{
    if (mapping_ == nullptr || mapping_ == MAP_FAILED)
        throw std::runtime_error("no mapping to sync");
    std::size_t bytes = size_ * sizeof(int);
    if (msync(mapping_, bytes, MS_SYNC) == -1)
    {
        throw std::runtime_error(std::string("msync failed: ") + std::strerror(errno));
    }
}
