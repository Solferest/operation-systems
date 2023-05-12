#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

int flag1 = 0;
int flag2 = 0;
sem_t semId;

void* proc1(void* flag1_void)
{
	int* flag1 = (int*)flag1_void;
	while (*flag1 != 1)
	{
		int try_value = sem_trywait(&semId);
		if (try_value == -1) {
			perror("sem_trywait");
			sleep(1);
			continue;
		}
		for (int i = 0; i < 10; i++)
		{
			putchar('1');
			fflush(stdout);
			sleep(1);
		}
		sem_post(&semId);
		sleep(1);
	}
	void* return_void = NULL;
	return return_void;
}

void* proc2(void* flag2_void)
{
	int* flag2 = (int*)flag2_void;
	while (*flag2 != 1)
	{
		int try_value = sem_trywait(&semId);
		if (try_value == -1) {
			perror("sem_trywait");
			sleep(1);
			continue;
		}
		for (int i = 0; i < 10; i++)
		{
			putchar('2');
			fflush(stdout);
			sleep(1);
		}
		sem_post(&semId);
		sleep(1);
	}
	void* return_void = NULL;
	return return_void;
}

int main() 
{
    pthread_t id1;
    pthread_t id2;
    sem_init(&semId, false, 1);
	int flag1 = 0;
	int flag2 = 0;
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
	sem_destroy(&semId);
	printf("\nпрограмма завершила работу\n");
}