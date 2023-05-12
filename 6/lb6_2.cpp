#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <sys/resource.h>
#include <errno.h>
#include <sys/mman.h>
#include <time.h>
#include <stdlib.h>
#include <cstring>
#include <signal.h>
using namespace std;

int flag = 0;
sem_t* semId_write;
sem_t* semId_read;
char loc[] = { "/memory" };
rlim_t* sharedMem;
char sem_name1[] = { "/write" };
char sem_name2[] = { "/read" };
int fd;
void sig_handler(int signo)
{
	sem_close(semId_read);
	sem_unlink(sem_name2);
	sem_close(semId_write);
	sem_unlink(sem_name1);
	munmap(sharedMem, sizeof(rlim_t));
	close(fd);
	shm_unlink(loc);
	exit(0);
}
void* proc(void* flag1_void)
{
	int* flag1 = (int*)flag1_void;
	rlim_t rlim;
	while (*flag1 != 1)
	{	
		sem_wait(semId_write);
		memcpy(&rlim, sharedMem, sizeof(rlim_t));
		cout << "soft limit:" << rlim << endl;
		sem_post(semId_read);
	}
	void* return_void = NULL;
	return return_void;
}

int main() 
{
	signal(SIGINT, sig_handler);
    pthread_t id1;
	fd = shm_open(loc, O_CREAT | O_RDWR, 0644);
	ftruncate(fd, sizeof(rlim_t));
	sharedMem = (rlim_t*)mmap(0, sizeof(rlim_t), PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
	semId_write = sem_open(sem_name1, O_CREAT, 0644, 0);
	semId_read = sem_open(sem_name2, O_CREAT, 0644, 0);
	pthread_create(&id1, NULL, proc, (void*)&flag);
	printf("программа ждет нажатия клавиши\n");
	getchar();
	printf("клавиша нажата\n");
	flag = 1;
	pthread_join(id1, NULL);
	sem_close(semId_read);
	sem_unlink(sem_name2);
	sem_close(semId_write);
	sem_unlink(sem_name1);
	munmap(sharedMem, sizeof(rlim_t));
	close(fd);
	shm_unlink(loc);
	printf("\nпрограмма завершила работу\n");
}