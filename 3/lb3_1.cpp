#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <sys/resource.h>
#include <pthread.h>
using namespace std;

int flag1 = 0;
int flag2 = 0;
int filedes[2];

void* proc1(void* flag1_void)
{
    int* flag1 = (int*)flag1_void;
    struct rlimit rlim;
    while (*flag1 != 1) {
        getrlimit(RLIMIT_CPU, &rlim);
        write(filedes[1], &(rlim.rlim_cur), sizeof(rlim_t));
        sleep(1);
    }
    void* return_void = NULL;
    return return_void;
}

void* proc2(void* flag2_void)
{
    int buf_size = 50;
    int* flag2 = (int*)flag2_void;
    while (*flag2 != 1) {
        rlim_t rlim;
        read(filedes[0], &rlim, sizeof(rlim_t));
        cout <<"soft limit:"<< rlim << endl;
        sleep(1);
    }
    void* return_void = NULL;
    return return_void;
}

int main() {
    pthread_t id1;
    pthread_t id2;
    pipe(filedes);
    pthread_create(&id1, NULL, proc1, (void*)&flag1);
    printf("поток 1 начал работу\n");
    pthread_create(&id2, NULL, proc2, (void*)&flag2);
    printf("поток 2 начал работу\n");
    printf("программа ждет нажатия клавиши\n");
    getchar();
    printf("клавиша нажата\n");
    flag1 = 1;
    flag2 = 1;
    pthread_join(id1, NULL);
    pthread_join(id2, NULL);
    close(filedes[0]);
    close(filedes[1]);
    printf("программа завершила работу\n");
}