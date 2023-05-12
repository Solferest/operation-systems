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
char fifoName[] = {"/tmp/my_named_pipe"};
pthread_t id_open;
pthread_t id_write;
size_t fd;
int write_cr=-1;

void* write_func(void* vvoid)
{
    struct rlimit rlim;
    while (flag != 1) {
        getrlimit(RLIMIT_CPU, &rlim);
        write(fd, &(rlim.rlim_cur), sizeof(rlim_t));
        sleep(1);
    }
    return NULL;
}
void* open_func(void* vvoid)
{
    while (flag != 1) {
        fd = open(fifoName, O_WRONLY|O_NONBLOCK);
        if (fd == -1) {
            cout << strerror(errno) << endl;
            sleep(1);
        }
        else {
            write_cr=pthread_create(&id_write, NULL, write_func, NULL);
            return NULL;
        }
    }
    return NULL;
}

void sig_handler(int signo) {
    cout << "get SIGPIPE\n";
}

int main() {
    signal(SIGPIPE, sig_handler);
    int make_fifo = mkfifo("/tmp/my_named_pipe", 0666);
    if (make_fifo==-1)cout << strerror(errno) << endl;
    pthread_create(&id_open, NULL, open_func, NULL);
    getchar();
    flag = 1;
    pthread_join(id_open, NULL);
    if (write_cr==0) pthread_join(id_write, NULL);
    close(fd);
    unlink(fifoName);
    return 0;
}