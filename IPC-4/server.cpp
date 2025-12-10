#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define MAX_CLIENTS 100
#define BUF_SIZE 1024
#define NAME_LEN 32

struct Client 
{
    int sock;
    std::string name;
};

std::vector<Client*> clients;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void broadcast(const std::string &message, int sender_sock) 
{
    pthread_mutex_lock(&clients_mutex);
    for (auto &cli : clients) 
    {
        if (cli->sock != sender_sock) 
	{
            send(cli->sock, message.c_str(), message.size(), 0);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void add_client(Client *cli) 
{
    pthread_mutex_lock(&clients_mutex);
    clients.push_back(cli);
    pthread_mutex_unlock(&clients_mutex);
}

void remove_client(int sock) 
{
    pthread_mutex_lock(&clients_mutex);
    for (auto it = clients.begin(); it != clients.end(); ++it) 
    {
        if ((*it)->sock == sock) 
	{
            delete *it;
            clients.erase(it);
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void *handle_client(void *arg) 
{
    Client *cli = (Client*)arg;
    char buf[BUF_SIZE];
    std::string msg;

    msg = cli->name + " connected.\n";
    broadcast(msg, cli->sock);

    while (true) 
    {
        int nbytes = recv(cli->sock, buf, sizeof(buf)-1, 0);
        if (nbytes <= 0) break;

        buf[nbytes] = '\0';
        std::string input(buf);

        if (input.substr(0,5) == "/exit") 
	{
            break;
        } 
	else if (input.substr(0,5) == "/list") 
	{
            std::string list = "users:\n";
            pthread_mutex_lock(&clients_mutex);
            for (auto &c : clients) 
	    {
                list += c->name + "\n";
            }
            pthread_mutex_unlock(&clients_mutex);
            send(cli->sock, list.c_str(), list.size(), 0);
        } 
	else 
	{
            msg = cli->name + ": " + input;
            broadcast(msg, cli->sock);
        }
    }

    msg = cli->name + " disconnected.\n";
    broadcast(msg, cli->sock);

    close(cli->sock);
    remove_client(cli->sock);
    pthread_exit(nullptr);
}

int main() 
{
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) { perror("socket"); return 1; }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_sock, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) { perror("bind"); return 1; }
    if (listen(server_sock, 10) < 0) { perror("listen"); return 1; }

    std::cout << "servere launched on port " << PORT << std::endl;

    while (true) 
    {
        sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        int client_sock = accept(server_sock, (sockaddr*)&client_addr, &addr_len);
        if (client_sock < 0) { perror("accept"); continue; }

        char name_buf[NAME_LEN];
        recv(client_sock, name_buf, NAME_LEN-1, 0);
        name_buf[strcspn(name_buf,"\n")] = 0;

        Client *cli = new Client{client_sock, std::string(name_buf)};
        add_client(cli);

        pthread_t tid;
        pthread_create(&tid, nullptr, handle_client, (void*)cli);
        pthread_detach(tid);
    }

    close(server_sock);
    return 0;
}

