#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
// Só para testar, no final a seed deve ser 0
#include <time.h>

#define LEN 7 + 1
#define N_SAMPLES 5

typedef struct {
    int size;
    int *elems;
} vector;

int check_unwanted(char *line) {
    if (line == NULL) {
        return EXIT_FAILURE;
    }

    for(int i = 0; i < sizeof(line); i++) {
        if (line[i] == '\n') {
            return 1;
        }
    }

    return 0;
}

int check_repeated(vector v, int elem) {
    if (v.elems == NULL || v.size < 0) {
        return EXIT_FAILURE;
    }

    if (v.size == 0) {
        return 0;
    }

    for(int i = 0; i < v.size; i++) {
        if (v.elems[i] == elem) {
            // printf("\n%d", elem);
            return 1;
        }
    }

    return 0;
}

int get_file_size(char *filename) {
    struct stat file_stats;
    if (stat(filename, &file_stats) == -1) {
        return EXIT_FAILURE;
    }

    return file_stats.st_size;
}

int main(int argc, char *argv[]) {
    srandom(time(NULL));
    FILE *fp;
    // provisório
    char *filename = "quote.txt";
    fp = fopen(filename, "r");

    char line[LEN];
    int jump;
    vector jumps;
    jumps.elems = malloc(sizeof(int) * N_SAMPLES);
    jumps.size = 0;

    for(int i = 0; i < N_SAMPLES; i++) {
        while(1) {
            jump = random() % (get_file_size(filename) - LEN);
            fseek(fp, jump, SEEK_SET);
            fgets(line, LEN, fp);

            if (check_repeated(jumps, jump) == 0 && 
            check_unwanted(line) == 0) {
                break;
            }
        }

        printf(">%s<\n", line);

        jumps.elems[i] = jump;
        jumps.size++;
    }

    fclose(fp);
    return EXIT_SUCCESS;
}