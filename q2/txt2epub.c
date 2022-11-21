#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define CMDLEN 256

int main(int argc, char *argv[]) {
    pid_t pid;
    char old_ext[CMDLEN];
    char new_ext[CMDLEN];
    char *target_name = "ebooks.zip";

    int i;
    for (i = 1; i < argc; i++) {
        strncpy(old_ext, argv[i], sizeof(char) * strlen(argv[i]));
        strncpy(new_ext, strtok(argv[i], "."), sizeof(char) * strlen(argv[i]) + 1);
        strncat(new_ext, ".epub", sizeof(char) * strlen(argv[i]) + 1);

        if ((pid = fork()) == 0) {
            if (execlp("pandoc", "pandoc", old_ext, "-o", new_ext, NULL) == -1) {
                perror("execlp");
                exit(EXIT_FAILURE);
            }
        } else {
            waitpid(pid, NULL, 0);
        }

        if ((pid = fork()) == 0) {
            execlp("zip", "zip", target_name, new_ext, NULL);
        } else {
            waitpid(pid, NULL, 0);
        }
    }
}