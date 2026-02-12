#pragma once
#include "Socket.hpp"
#include "ThreadPool.hpp"
#include <functional>
#include <thread>

namespace SimpleNet 
{

using ClientHandler = std::function<void(Socket)>;

class TcpServer 
{
public:
    TcpServer(int port, size_t thread_count = std::thread::hardware_concurrency());
    void run(ClientHandler handler);

private:
    Socket listen_socket_;
    ThreadPool pool;
};

} // namespace SimpleNet

