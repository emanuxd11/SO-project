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
#include <signal.h>

#define NAME_LEN 256

int token = 0;
int fd_write;
int fd_read;
char **fifoarr;
int n;

// Checks probability
int probability(double p) {
    double random = (double)rand() / RAND_MAX;
    if (random <= p) {
        return 1;
    }

    return 0;
}

void my_handler(int s) {
    //unlinks fifos
    for(int i = 0; i < n; i++) {
           unlink(fifoarr[i]);
    }
    wait(NULL);

    //frees fifo array
    for (int i = 0; i < n; i++) {
        free(fifoarr[i]);
    }
    free(fifoarr);

    // kills child processes
    kill(0, SIGKILL);

    exit(EXIT_SUCCESS);
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

    n = atoi(argv[1]);
    if (n < 2) {
        fprintf(stderr, "%s: need at least 2 pipes, %d given", argv[0], n);
        exit(EXIT_FAILURE);
    }

    double p = atof(argv[2]);
    if (p < 0 || p > 1) {
        fprintf(stderr, "%s: expected probability value between 0 and 1, %lf given", argv[0], p);
        exit(EXIT_FAILURE);
    }
    
    int t = atoi(argv[3]);
    if (t < 0) {
        fprintf(stderr, "%s: expected positive time value, %d given", argv[0], t);
        exit(EXIT_FAILURE);
    }

    // make the fifos
    char fifoname[NAME_LEN];
    fifoarr = malloc(n * sizeof(char *));

    pid_t pid;

    for (int i = 1, j = 0; i <= n; i++, j++) {
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

    //Create the child processes and the ring
    for (int i = 1, j = 0; i <= n; i++, j++) {
        if ((pid = fork()) < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        // Creates a different seed for each process
        srand(getpid());

        if (pid == 0) {
            // Child Process
            if (i == 1) {
                // Creates the first part of the ring
                if ((fd_read = open(fifoarr[0], O_RDONLY)) < 0) {
                    perror("fd_read error");
                }
                if ((fd_write = open(fifoarr[1], O_WRONLY)) < 0) {
                    perror("fd_write error");
                }

                if (write(fd_write, &token, sizeof(int)) < 0) {
                    perror("write error");
                }
            } else if(i == n) {
                // Close the ring
                if((fd_write = open(fifoarr[0],O_WRONLY)) < 0) {
                    perror("fd_write error");
                }
                
                if((fd_read = open(fifoarr[j], O_RDONLY)) < 0) {
                   perror("fd_read error");
                }
            } else {
                // Creates the middle of the ring
                if ((fd_write = open(fifoarr[j+1],O_WRONLY)) < 0) {
                    perror("fd_write error");
                }

                if ((fd_read = open(fifoarr[j], O_RDONLY)) < 0) {
                    perror("fd_read error");
                }
            }
            
            // Infinite loop where token is passed through the ring
            for ( ; ; ) {
                if (read(fd_read, &token, sizeof(token)) < 0) {
                    perror("read error");
                }

                // Checks probability, enters if it is whitin bounds
                if (probability(p)) {
                    fprintf(stdout, "[p%d] lock on token (val = %d)\n", i, token);
                    sleep(t);
                    fprintf(stdout, "[p%d] unlocked token\n", i);
                }

                token++;
                if (write(fd_write, &token, sizeof(token)) < 0) {
                    perror("write error");
                    exit(EXIT_FAILURE);
                }

                // Waits for a CTRL+C event
                if (signal(SIGINT, my_handler) == SIG_ERR) {
                    perror("signal error");
                    exit(EXIT_FAILURE);
                }
            }
        }
    }

    // waits for all child processes to terminate
    for (int i = 0; i < n; i++) {
        if (wait(NULL) < 0) {
            perror("wait error");
            exit(EXIT_FAILURE);
        }
    }
}
