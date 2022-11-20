#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define CMDLEN 256

int main(int argc, char *argv[]) {
    pid_t pid;
    char old_ext[CMDLEN + 1];
    char new_ext[CMDLEN + 1];

    char **epub_files;
    epub_files = malloc((argc + 1) * sizeof(char *));
    epub_files[0] = "ebooks.zip";

    int i;
    for (i = 1; i < argc; i++) {
        strncpy(old_ext, argv[i], CMDLEN);
        strncpy(new_ext, strtok(argv[i], "."), CMDLEN);
        strncat(new_ext, ".epub", CMDLEN);

        // rever isto
        epub_files[i] = new_ext;

        if ((pid = fork()) == 0) {
            execlp("pandoc", "pandoc", old_ext, "-o", new_ext, NULL);
        }

        waitpid(pid, NULL, 0);
    }
    epub_files[i] = NULL;

    for (int j = 0; j < i; j++) {
        printf("%s\n", epub_files[j]);
    }

    if ((pid = fork()) == 0) {
        execvp("zip", epub_files);
    } else {
        waitpid(pid, NULL, 0);
    }

    free(epub_files);
}