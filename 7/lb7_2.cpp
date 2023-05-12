#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>
#include <iostream>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/mman.h>
#include <cstring>
using namespace std;

int flag = 0;
size_t fd;


void* read_func(void* vvoid)
{
    while (flag != 1) {
        rlim_t rlim;
        int rv=read(fd, &rlim, sizeof(rlim_t));
        if (rv>0)cout << "soft limit:" << rlim << endl;
        else if (rv==0) cout<<"Информация не записана" << endl;
        else cout << strerror(errno) << endl;
        sleep(1);
    }
    return NULL;
}

int main() {
    pthread_t id_read;
    char fifoName[] = { "/tmp/my_named_pipe" };
    int make_fifo = mkfifo(fifoName, 0644);
    if (make_fifo == -1)cout << strerror(errno) << endl;
    fd = open(fifoName, O_RDONLY | O_NONBLOCK);
    if (fd == -1)cout << strerror(errno) << endl;
    pthread_create(&id_read, NULL, read_func, NULL);
    getchar();
    flag = 1;
    pthread_join(id_read, NULL);
    close(fd);
    unlink(fifoName);
    return 0;
}