#include "TcpServer.hpp"

namespace SimpleNet 
{

TcpServer::TcpServer(int port, size_t thread_count)
    : listen_socket_(), pool(thread_count)
{
    listen_socket_.bind(port);
    listen_socket_.listen();
}

void TcpServer::run(ClientHandler handler) 
{
    while (true) 
    {
        Socket client = listen_socket_.accept();

        auto client_ptr = std::make_shared<Socket>(std::move(client));

        pool.enqueue([client_ptr, handler]() 
	    {
                handler(std::move(*client_ptr));
            }
        );
    }
}

} // namespace SimpleNet

