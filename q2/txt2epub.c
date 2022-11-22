#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define CMDLEN 256

int main(int argc, char *argv[]) {
    pid_t pid;

    // para guardar o nome do ficheiro com a
    // antiga extensão e com a nova extensão
    char old_ext[CMDLEN];
    char new_ext[CMDLEN];

    // alocar espaço para argc ficheiros, o nome do zip e o próprio comando
    char **epub_files;
    epub_files = malloc((argc + 2) * sizeof(char *));

    // inicialização dos primeiros dois elementos do array de comandos
    epub_files[0] = "zip";
    epub_files[1] = "ebooks.zip";

    // ciclo dos forks
    for (int i = 1, j = 2; i < argc; i++, j++) {
        // caso o nome do ficheiro seja demasiado
        // grande, não é convertido e compactado
        if (strlen(argv[i]) > CMDLEN - 2) {
            fprintf(stderr, "file number %d won't be" 
            "added because filename is too large", i);
            continue;
        }
        
        // conversão de .txt para .epub
        strcpy(old_ext, argv[i]);
        strcpy(new_ext, strtok(argv[i], "."));
        strcat(new_ext, ".epub");

        // alocar memória para o próximo nome de ficheiro        
        epub_files[j] = malloc(CMDLEN * sizeof(char));
        // adiconar ao array o ficheiro com a extensão epub
        strcpy(epub_files[j], new_ext);

        if ((pid = fork()) < 0) {
            // em caso de erro
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // processo filho
            if (execlp("pandoc", "pandoc", old_ext, "-o", new_ext, NULL) == -1) {
                perror("execlp");
                exit(EXIT_FAILURE);
            }
        } else {
            // a função do processo pai é criar mais
            // processos filhos que executem o pandoc
            continue;
        }
    }
    epub_files[argc + 1] = NULL;

    // ciclo dos waits
    for (int i = 1; i < argc; i++) {
        if (wait(NULL) < 0) {
            fprintf(stderr, "%s: wait error: %s\n",
            argv[0], strerror(errno));
        }
    }

    // execução do zip
    if ((pid = fork()) < 0) {
        // em caso de erro
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // o processo filho executa o comando zip
        if (execvp("zip", epub_files) == -1) {
            perror("execvp");
            exit(EXIT_FAILURE);
        }
    } else {
        // o processo pai espera que o 
        // filho termine de executar
        if (waitpid(pid, NULL, 0) < 0) {
            fprintf(stderr, "%s: waitpid error: %s\n",
            argv[0], strerror(errno));
        }
    }

    

    // exit gracefully xD
    exit(EXIT_SUCCESS);
}