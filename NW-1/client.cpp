#include "Socket.hpp"
#include <iostream>

int main() 
{
    SimpleNet::Socket client;
    client.connect("127.0.0.1", 1111);
    client.send("Hello server");

    auto response = client.receive();
    std::string msg(response.begin(), response.end());
    std::cout << "Server responded: " << msg << "\n";
}

