#include <iostream>
#include <string>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define BUF_SIZE 1024
#define NAME_LEN 32

int sock;

void* recv_handler(void*) 
{
    char buf[BUF_SIZE];
    while (true) 
    {
        int nbytes = recv(sock, buf, sizeof(buf)-1, 0);
        if (nbytes <= 0) break;
        buf[nbytes] = '\0';
        std::cout << buf;
    }
    std::cout << "connection closed.\n";
    exit(0);
}

int main() 
{
    sockaddr_in server_addr;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) { perror("socket"); return 1; }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) { perror("connect"); return 1; }

    std::string name;
    std::cout << "enter username: ";
    std::getline(std::cin, name);
    send(sock, name.c_str(), name.size(), 0);

    pthread_t tid;
    pthread_create(&tid, nullptr, recv_handler, nullptr);

    std::string msg;
    while (true) 
    {
        std::getline(std::cin, msg);
        send(sock, msg.c_str(), msg.size(), 0);
        if (msg.substr(0,5) == "/exit") break;
    }

    close(sock);
    return 0;
}

