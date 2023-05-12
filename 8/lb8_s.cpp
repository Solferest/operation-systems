#include <iostream>
#include <string>
#include <queue>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <string.h>
#include <grp.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/resource.h>
#include <sys/mman.h>
#include <vector>

using namespace std;

int listen_socket;
int client_socket;
queue<string> que;
int flag_get = 0;
int flag_send = 0;
int flag_wait = 0;
pthread_mutex_t mx;
pthread_t getId;
pthread_t sendId;
struct sockaddr_in serverAddr;
int get = -1;
int ifsend = -1;


void* server_send(void* vvoid)
{
    struct rlimit rlim;
    while (flag_send != 1)
    {
        pthread_mutex_lock(&mx);
        if (!que.empty())
        {
            string message = que.front();
            que.pop();
            cout << message <<" recieved for processing"<< endl;
            pthread_mutex_unlock(&mx);
            getrlimit(RLIMIT_CPU, &rlim);
            string response = to_string(rlim.rlim_cur);
            int sendcount = send(client_socket, &(rlim.rlim_cur), sizeof(rlim.rlim_cur), 0);
            if (sendcount == -1) perror("send");
            else cout << "Response for "<< message << " sent" << endl;
        }
        else
        {
            pthread_mutex_unlock(&mx);
            sleep(1);
        }

    }
    return NULL;
}


void* server_get(void* vvoid)
{
    const int RCV_SIZE = 1024;
    while (flag_get != 1)
    {
        char buf[RCV_SIZE];
        string rcv;
        int reccount = recv(client_socket, &buf, RCV_SIZE, 0);
        if (reccount == -1)
        {
            perror("recv");
            sleep(1);
        }
        else if (reccount == 0)
        {
            sleep(1);
        }
        else
        {
            rcv = string(buf);
            cout << rcv <<" recieved" << endl;
            pthread_mutex_lock(&mx);
            que.push(rcv);
            pthread_mutex_unlock(&mx);
        }
    }
    return NULL;
}

void sig_handler(int signo) {
    cout << "get SIGPIPE\n";
}

void* server_wait(void* vvioid)
{
    while (flag_wait != 1)
    {
        int addrlen = sizeof(serverAddr);
        client_socket = accept(listen_socket, (struct sockaddr*)NULL, NULL);
        if (client_socket == -1)
        {
            perror("accept");
            sleep(1);
        }
        else
        {
            cout << "Client connected" << endl;
            struct sockaddr_in sin;
            socklen_t len = sizeof(sin);
            if (getsockname(client_socket, (struct sockaddr*)&sin, &len) == -1)
                perror("getsockname");
            else
                printf("client port number %d\n", ntohs(sin.sin_port));
            pthread_create(&getId, NULL, server_get, NULL);
            pthread_create(&sendId, NULL, server_send, NULL);
            pthread_join(sendId, NULL);
            pthread_join(getId, NULL);
        }
    }
    shutdown(client_socket, 2);
    close(client_socket);
    return NULL;
}


int main()
{
    signal(SIGPIPE, sig_handler);
    pthread_t waitId;
    listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(listen_socket, F_SETFL, O_NONBLOCK);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(7000);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    bind(listen_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    int optval = 1;
    setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    listen(listen_socket, SOMAXCONN);
    cout << "Server started" << endl;
    pthread_mutex_init(&mx, NULL);
    pthread_create(&waitId, NULL, server_wait, NULL);
    getchar();
    flag_get = 1;
    flag_send = 1;
    flag_wait = 1;
    pthread_join(waitId, NULL);
    cout << "Server finished" << endl;
    pthread_mutex_destroy(&mx);
    close(listen_socket);
    return 0;
}