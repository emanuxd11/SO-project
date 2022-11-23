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
    fifoarr = malloc(n * sizeof(char *));

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

        /* if everything goes well add the fifo name to the array */
        fifoarr[j] = malloc((strlen(fifoname) + 1) * sizeof(char));
        strcpy(fifoarr[j], fifoname);
    }


    pid_t pid;
    int fd_write;
    int fd_read;

    for(unsigned int i = 1; i <= n; i++){
        if((pid = fork() < 0)){
            perror("fork");
        }
        else if(pid == 0){
            /*Child Process*/
            sprintf(fifoarr[i], "fifo%dto%d", i, i+1);
            if((fd_write = open(fifoarr[i],O_WRONLY)) < 0){
                perror("fd_write error");
            }
        }
        else{
            /*Parent process*/
            sprintf(fifoarr[i], "fifo%dto%d", i, i+1);
            if((fd_read = open(fifoarr[i], O_RDONLY)) < 0){
                perror("fd_read error");
            }
        }
    }

    /*while(1){
        if(pid == 0){
            //Child Process

        }
        else if(pid > 0){
            //Parent Process
        }
        else{
            perror("fork");
        }
    }*/
    for(unsigned int i = 0; i < n; i++){
        printf("%s\n", fifoarr[i]);
        free(fifoarr[i]);
    }
    free(fifoarr);
}