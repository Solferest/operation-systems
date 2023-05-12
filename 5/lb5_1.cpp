#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int flag = 0;
sem_t* semId;
FILE* file_name;

void* proc(void* flag1_void)
{
	int* flag1 = (int*)flag1_void;
	while (*flag1 != 1)
	{
		sem_wait(semId);
		for (int i = 0; i < 10; i++)
		{
			putchar('1');
			fprintf(file_name, "%c", '1');
			fflush(stdout);
			fflush(file_name);
			sleep(1);
		}
		sem_post(semId);
		sleep(1);
	}
	void* return_void = NULL;
	return return_void;
}

int main() 
{
    pthread_t id1;
    pthread_t id2;
	char sem_name[] = { "/name" };
	semId = sem_open(sem_name, O_CREAT, 0644, 1);
	file_name = fopen("data.txt", "a+");
	pthread_create(&id1, NULL, proc, (void*)&flag);
	printf("программа ждет нажатия клавиши\n");
	getchar();
	printf("клавиша нажата\n");
	flag = 1;
	pthread_join(id1, NULL);
	pthread_join(id2, NULL);
	fclose(file_name);
	sem_close(semId);
	sem_unlink(sem_name);
	printf("\nпрограмма завершила работу\n");
}