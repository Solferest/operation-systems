#include <stdio.h>
#include <unistd.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(int arg_count, char* argv[]) {
    printf("Индификатор текущего процесса: %d\n", getppid());
    printf("Индификатор родительского процесса: %d\n", getpid());
    pid_t pid = fork();
    if (pid != 0) 
    {
        int status;
        while (waitpid(-1, &status, WNOHANG) == 0) {
            printf("Ждем\n");
            usleep(500000);
        }
        if (WIFEXITED(status))
            printf("Дочерний процесс завершился со статусом: %d\n", WEXITSTATUS(status));
        else
            printf("Ошибка\n");
    }
    else if (pid==0)
    {
        setenv("PATH", "/home/elizaveta", 1);
        execvp("lb4_1", argv);
    }
    else perror("fork");
    return 0;
}