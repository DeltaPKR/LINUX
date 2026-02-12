#include "TcpServer.hpp"
#include "Socket.hpp"
#include <iostream>

int main() 
{
    SimpleNet::TcpServer server(1111);

    server.run([](SimpleNet::Socket client) 
    {
        auto data = client.receive();
        std::string msg(data.begin(), data.end());
        std::cout << "Received: " << msg << "\n";
        client.send("Echo: " + msg);
    });
}

