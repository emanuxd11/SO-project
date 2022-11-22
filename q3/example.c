#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

int main (int argc, char *argv[]) {
    if (mkfifo("myfifo", 0755) == -1) {
        if (errno != EEXIST) {
            fprintf(stderr, "Could not create new fifo file!\n");
            return 1;
        }
    }
    
    int pid; 
    if((pid = fork()) < 0) {
        perror("fork");
    } else if (pid == 0) {
        /* this is child */
        printf("READING - Opening...\n");
        int fd = open("myfifo", O_RDONLY);
        printf("READING - Opened\n");
        int x;
        printf("READING - Reading...\n");
        if (read(fd, &x, sizeof(x)) == -1) {
            return 3;
        }
        printf("READING - Read, value of x is %d\n", x);
        printf("READING - Closing...\n");
        close(fd);
        printf("READING - Closed\n");

    } else {
        /* this is parent */
        printf("WRITING - Opening...\n");
        int fd = open("myfifo", O_WRONLY);
        printf("WRITING - Opened\n");
        int x = 97;
        printf("WRITING - Writing...\n");
        if (write(fd, &x, sizeof(x)) == -1) {
            return 2;
        }
        printf("WRITING - Wrote\n");
        printf("WRITING - Closing...\n");
        close(fd);
        printf("WRITING - Closed\n");
    }

    return 0;
}