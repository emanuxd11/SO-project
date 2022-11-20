#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>


int main() {
    pid_t pid;
    char *commands[] = {"ls", "tree"};


    if ((pid = fork()) == 0) {
        execlp(commands, commands, (char *)0);
    }

    waitpid(pid, NULL, 0);

    printf("ac");
}