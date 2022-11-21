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

    for (int i = 1; i < argc; i++) {
        // para não usar strncpy e strncat, acho que é melhor 
        // verificar antes se o tamanho não é demasiado grande
        if (strlen(argv[i]) > CMDLEN - 2) {
            fprintf(stderr, "file number %d won't be" 
            "added because filename is too large", i);
            continue;
        }

        // aqui já seria seguro usar o strcpy e strcat
        // porque já garantimos que a memória é suficiente
        strcpy(old_ext, argv[i]);
        strcpy(new_ext, strtok(argv[i], "."));
        strcat(new_ext, ".epub");

        // primeiro fork para executar o pandoc
        if ((pid = fork()) == 0) {
            if (execlp("pandoc", "pandoc", old_ext, "-o", new_ext, NULL) == -1) {
                perror("execlp");
                exit(EXIT_FAILURE);
            }
        } else {
            // o processo pai espera que o processo
            // filho termine a execução do pandoc
            if (waitpid(pid, NULL, 0) < 0) {
                fprintf(stderr, "%s: waitpid error: %s\n",
                argv[0], strerror(errno));
            }
        }

        // primeiro fork para executar o zip
        if ((pid = fork()) == 0) {
            if (execlp("zip", "zip", target_name, new_ext, NULL) == -1) {
                perror("execlp");
                exit(EXIT_FAILURE);
            }
        } else {
            // o processo pai espera que o processo
            // filho termine a execução do pandoc
            if (waitpid(pid, NULL, 0) < 0) {
                fprintf(stderr, "%s: waitpid error: %s\n",
                argv[0], strerror(errno));
            }
        }
    }

    exit(EXIT_SUCCESS);
}