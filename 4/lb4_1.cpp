#include <stdio.h>
#include <unistd.h>

int main(int arg_count, char* argv[]) {
    printf("Индификатор родительского процесса:: %d\n", getppid());
    printf("Индификатор текущего процесса:: %d\n", getpid());
    for (int i = 0; i < arg_count; i++) {
        sleep(1);
        printf("%s \n", argv[i]);
    }
    return 10;
}