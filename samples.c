#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

typedef struct {
    int size;
    int *elems;
} vector;

/*
    Retorna -2 em caso de erro, o índice 
    em que se encontra o caracter indesejado, 
    caso este exista, ou -1 caso não haja nenhum
    erro nem nenhum caracter indesejado.
*/
int check_unwanted(char *line) {
    if (line == NULL) {
        return -2;
    }

    for(int i = 0; i < strlen(line); i++) {
        if (line[i] == '\n') {
            line[i] = ' ';
            return i;
        }    
    }

    return -1;
}

/*
    Retorna -1 em caso de erro, 0 caso 
    o valor inserido já exista no vetor,
    ou 1 caso não exista. 
*/
int check_repeated(vector v, int elem) {
    if (v.elems == NULL || v.size < 0) {
        fprintf(stderr, "Error: invalid vector");
        return -1;
    }

    if (v.size == 0) {
        return 0;
    }

    for(int i = 0; i < v.size; i++) {
        if (v.elems[i] == elem) {
            return 1;
        }
    }

    return 0;
}

/*
    Retorna o tamanho do ficheiro ou -1
    em caso de erro.
*/
int get_file_size(char *filename) {
    struct stat file_stats;
    if (stat(filename, &file_stats) == -1) {
        perror("Error");
        return -1;
    }

    return file_stats.st_size;
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "%s: missing arguments", argv[0]);
        return EXIT_FAILURE;
    }

    srandom(0);
    FILE *fp;
    char *filename = argv[1];
    int n_samples = atoi(argv[2]), sample_len = atoi(argv[3]) + 1;

    if ((fp = fopen(filename, "r")) == NULL) {
        fprintf(stderr, "Error: %s", strerror(errno));
        return EXIT_FAILURE;
    }

    char line[sample_len];
    int jump;
    vector jumps;

    int file_size = get_file_size(filename);
    if (file_size == -1) {
        return EXIT_FAILURE;
    }

    jumps.elems = malloc(sizeof(int) * n_samples);
    jumps.size = 0;
    if (jumps.elems == NULL) {
        fprintf(stderr, "Error: couldn't allocate memory: malloc");
        return EXIT_FAILURE;
    }

    for(int i = 0; i < n_samples; i++) {
        while(1) {
            jump = random() % (file_size - sample_len);
            
            int status = check_repeated(jumps, jump);
            if (status == 0) {
                break;
            } else if (status == -1) {
                free(jumps.elems);
                return EXIT_FAILURE;
            }
        }
        
        while(1) {
            fseek(fp, jump, SEEK_SET);
            fgets(line, sample_len, fp);

            int pos = check_unwanted(line);
            if (pos == -1) {
                break;
            }
            if (pos == -2) {
                free(jumps.elems);
                fprintf(stderr, "Error: invalid line");
                return EXIT_FAILURE;
            }

            char next_line[sample_len];
            fgets(next_line, sample_len - (pos + 1), fp);
            strcat(line, next_line);
            
            break;
        }
        
        printf(">%s<\n", line);

        jumps.elems[i] = jump;
        jumps.size++;
    }

    free(jumps.elems);
    fclose(fp);
    return EXIT_SUCCESS;
}