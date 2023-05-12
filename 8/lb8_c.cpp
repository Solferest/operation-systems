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
#include <errno.h>
#include <string.h>
#include <grp.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/resource.h>
#include <sys/mman.h>
using namespace std;

int flag_get = 0;
int flag_wait = 0;
int flag_send = 0;
int client_socket;
struct sockaddr_in clientSockAddr;
pthread_t sendId;
pthread_t getId;
int ifsend = -1, get = -1;

void sig_handler(int fd)
{
    cout << "Client disconnected with server" << endl;
    sleep(1);
}

void* client_get(void* vvoid)
{
    errno = 0;
    while (flag_get != 1)
    {
        rlim_t rlim;
        int reccount = recv(client_socket, &rlim, sizeof(rlim), 0);
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
            cout << "Response: "<< rlim;
            cout << endl;
            sleep(1);
        }
    }
    return NULL;
}


void* client_send(void* vvoid)
{
    int count = 1;
    while (flag_send != 1)
    {
        string buf = "Request №";
        buf += to_string(count);
        char buff[20];
        strcpy(buff, buf.c_str());
        int sendcount = send(client_socket, &buff, sizeof(buff), MSG_DONTWAIT);
        cout << buf;
        if (sendcount == -1)
        {
            perror("send");
        }
        else
        {
            cout << " sent" << endl;
            ++count;
        }
        sleep(1);
    }
    return NULL;
}


void* client_wait(void* vvoid)
{

    while (flag_wait != 1)
    {
        int result = connect(client_socket, (struct sockaddr*)&clientSockAddr, sizeof(clientSockAddr));
        
        if (result == -1)
        {
            perror("connect");
            sleep(1);
        }
        else
        {
            cout << "Connection with server established" << endl;
            struct sockaddr_in sin;
            socklen_t len = sizeof(sin);
            if (getsockname(client_socket, (struct sockaddr*)&sin, &len) == -1)
                perror("getsockname");
            else
                printf("client port number %d\n", ntohs(sin.sin_port));
            pthread_create(&sendId, NULL, client_send, NULL);
            pthread_create(&getId, NULL, client_get, NULL);
            pthread_join(sendId, NULL);
            pthread_join(getId, NULL);
        }
    }
    return NULL;
}


int main()
{
    signal(SIGPIPE, sig_handler);
    pthread_t waitId;
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(client_socket, F_SETFL, O_NONBLOCK);
    clientSockAddr.sin_family = AF_INET;
    clientSockAddr.sin_port = htons(7000);
    clientSockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    cout << "Client started" << endl;
    pthread_create(&waitId, NULL, client_wait, NULL);
    getchar();
    flag_wait = 1;
    flag_get = 1;
    flag_send = 1;
    pthread_join(waitId, NULL);
    cout << "Client finished" << endl;
    shutdown(client_socket, 2);
    close(client_socket);
    return 0;
}