#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>

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
    for (unsigned int i = 1; i <= n; i++) {
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
        fifoarr[i - 1] = fifoname;
    }


    free(fifoarr);
}