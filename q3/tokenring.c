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
#include <time.h>

#define NAME_LEN 256

int token = 0;
int fd_write;
int fd_read;

int __roda_do_preco_certo_69(double p) {
    double random = (double)rand() / RAND_MAX;
    if (random <= p) {
        return 1;
    }

    return 0;
}

int main (int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "%s: 3 arguments expected, %d given", argv[0], argc - 1);
        exit(EXIT_FAILURE);
    } else if (argc > 4) {
        fprintf(stderr, "%s: 3 arguments expected, %d given. Everything "
        "after the third argument will be ignored", argv[0], argc - 1);
        argc = 4;
    }

    int n = atoi(argv[1]);
    if (n < 2) {
        fprintf(stderr, "%s: need at least 2 pipes, %d given", argv[0], n);
        exit(EXIT_FAILURE);
    }
    double p = atof(argv[2]);
    int t = atoi(argv[3]);

    // make the fifos
    char fifoname[NAME_LEN];
    char **fifoarr;
    fifoarr = malloc(n * sizeof(char *));

    pid_t pid;

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

    for (unsigned int i = 1, j = 0; i <= n; i++, j++) {
        if ((pid = fork()) < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        srand(getpid());

        if (pid == 0) {
            //Child Process
            if (i == 1) {
               if ((fd_read = open(fifoarr[0], O_RDONLY)) < 0) {
                   perror("fd_read error");
               }
               if ((fd_write = open(fifoarr[1], O_WRONLY)) < 0) {
                   perror("fd_write error");
               }
               write(fd_write, &token, sizeof(int));
            } else if(i == n) {
                puts("opening for write");
                if((fd_write = open(fifoarr[0],O_WRONLY)) < 0) {
                    perror("fd_write error");
                }
                puts("opening for read");
                if((fd_read = open(fifoarr[j], O_RDONLY)) < 0) {
                   perror("fd_read error");
                }
            } else {
                if ((fd_write = open(fifoarr[j+1],O_WRONLY)) < 0) {
                    perror("fd_write error");
                }

                if ((fd_read = open(fifoarr[j], O_RDONLY)) < 0) {
                    perror("fd_read error");
                }
            }
            
            for ( ; ; ) {
                read(fd_read, &token, sizeof(token));

                if (__roda_do_preco_certo_69(p)) {
                    printf("[p%d] lock on token (val = %d)\n", i, token);
                    sleep(t);
                    printf("[p%d] unlocked token\n", i);
                }

                token++;
                write(fd_write, &token, sizeof(token));
            }
        }
    }

    for (unsigned int i = 0; i < n; i++) {
        wait(NULL);
    }

    for (unsigned int i = 0; i < n; i++) {
        free(fifoarr[i]);
    }
    free(fifoarr);
}