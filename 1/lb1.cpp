#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

struct targs
{
	int flag;
	int sec;
};

void* proc1(void* arg1)
{
	targs* arg = (targs*)arg1;
	while (arg->flag !=1)
	{
		putchar('1');
		fflush(stdout);
		sleep(arg->sec);
	}
	int* ret = new int;
	*ret = 3;
	pthread_exit(ret);
}
void* proc2(void* arg2)
{
	targs* arg = (targs*)arg2;
	while (arg->flag != 1)
	{
		putchar('2');
		fflush(stdout);
		sleep(arg->sec);
	}
	int* ret = new int;
	*ret = 4;
	pthread_exit(ret);
}

int main()
{
	printf("программа начала работу\n");
	pthread_t id1;
	pthread_t id2;
	targs arg1;
	targs arg2;
	arg1.flag = 0;
	arg1.sec = 1;
	arg2.flag = 0;
	arg2.sec = 1;
	pthread_create(&id1, NULL, proc1, (void*)&arg1);
	printf("поток 1 начал работу\n");
	pthread_create(&id2, NULL, proc2, (void*)&arg2);
	printf("поток 2 начал работу\n");
	printf("программа ждет нажатия клавиши\n");
	getchar();
	printf("клавиша нажата\n");
	arg1.flag = 1;
	arg2.flag = 1;
	int* exitcode1;
	int* exitcode2;
	pthread_join(id1, (void**)&exitcode1);
	pthread_join(id2, (void**)&exitcode2);
	printf("exitcode1 = %d\n", *exitcode1);
	printf("exitcode2 = %d\n", *exitcode2);
	printf("программа завершила работу\n");
	delete exitcode1;
	delete exitcode2;
}