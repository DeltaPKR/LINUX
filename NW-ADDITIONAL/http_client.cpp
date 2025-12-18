#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <netdb.h>

// g++ -std=c++17 http_client.cpp -o http_client

int main()
{
    const char * host = "httpforever.com";

    hostent * he = gethostbyname(host);
    if (!he)
    {
        std::cerr << "DNS resolve error\n";
        return 1;
    }

    in_addr ** addr_list = (in_addr **)he->h_addr_list;
    if (!addr_list[0])
    {
        std::cerr << "No IP addresses found\n";
        return 1;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        std::cerr << "socket error\n";
        return 1;
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    addr.sin_addr = *addr_list[0];

    if (connect(sock, (sockaddr *)&addr, sizeof(addr)) < 0)
    {
        std::cerr << "connect error\n";
        close(sock);
        return 1;
    }

    std::string request =
        "GET / HTTP/1.1\r\n"
        "Host: httpforever.com\r\n"
        "Connection: close\r\n"
        "\r\n";

    if (send(sock, request.c_str(), request.size(), 0) < 0)
    {
        std::cerr << "send error\n";
        close(sock);
        return 1;
    }

    int fd = open("httpforever.html", O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd < 0)
    {
        std::cerr << "file open error\n";
        close(sock);
        return 1;
    }

    char buffer[4096];

    while (true)
    {
        ssize_t n = recv(sock, buffer, sizeof(buffer), 0);
        if (n <= 0) break;

        write(fd, buffer, n);
    }

    close(fd);
    close(sock);

    std::cout << "Saved to httpforever.html\n";
    return 0;
}

