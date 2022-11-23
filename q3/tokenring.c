#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>

#define NAME_LEN 256

unsigned int x;

int main (int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "%s: 3 arguments expected, %d given", argv[0], argc - 1);
        exit(EXIT_FAILURE);
    } else if (argc > 4) {
        fprintf(stderr, "%s: 3 arguments expected, %d given. Everything "
        "after the third argument will be ignored", argv[0], argc - 1);
        argc = 4;
    }

    unsigned int n = atoi(argv[1]);
    double p = atof(argv[2]);
    int t = atoi(argv[3]);

    /* make the fifos */
    char fifoname[NAME_LEN];
    char **fifoarr;
    fifoarr = malloc((n + 1) * sizeof(char *));

    for (unsigned int i = 1, j = 0; i <= n; i++, j++) {
        if (i == n) {
            sprintf(fifoname, "pipe%dto%d", i, 1);
        } else {
            sprintf(fifoname, "pipe%dto%d", i, i + 1);
        }

        if (mkfifo(fifoname, 0666) == -1) {
            if (errno != EEXIST) {
                fprintf(stderr, "Could not create new fifo file!\n");
                exit(EXIT_FAILURE);
            }
        }

        // if everything goes well add the fifo name to the array
        fifoarr[j] = malloc((strlen(fifoname) + 1) * sizeof(char));
        strcpy(fifoarr[j], fifoname);
    }
    fifoarr[n] = malloc((strlen(fifoarr[0]) + 1) * sizeof(char));
    strcpy(fifoarr[n], fifoarr[0]);

    pid_t pid;
    int fd_write;
    int fd_read;
        
    for (unsigned int i = 1, j = 0; i <= n; i++, j++) {
        if ((pid = fork()) < 0) {
            perror("fork");
        }
        
        if (pid == 0) {
            // Child Process
            if ((fd_write = open(fifoarr[j + 1], O_WRONLY)) < 0) {
                perror("fd_write error");
            }
            printf("[pid%d] opened %s for writing\n", getpid(), fifoarr[j + 1]);

            if (i == 1) {
                x = -1;
                write(fd_write, &x, sizeof(x));       
                printf("[pid%d] wrote x = %u\n", getpid(), x);
            }

            for ( ; ; ) {
                read(fd_read, &x, sizeof(x));
                printf("[pid%d] x = %u\n", getpid(), x);
                sleep(5);
                x++;
                write(fd_write, &x, sizeof(x));
            }
        }

        if ((fd_read = open(fifoarr[j], O_RDONLY)) < 0) {
            perror("fd_read error");
        }
        printf("[pid%d] opened %s for reading\n", getpid(), fifoarr[j]);
    }

    for (unsigned int i = 0; i < n; i++) {
        wait(NULL);
    }

    for (unsigned int i = 0; i < n; i++) {
        free(fifoarr[i]);
    }
    free(fifoarr);
}