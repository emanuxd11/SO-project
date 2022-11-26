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

    for(size_t i = 0; i < strlen(line); i++) {
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

/*
    Obter o valor do salto. Retorna esse valor, ou termina
    o programa, libertando toda a memória dinamicamente
    alocada.
*/
int get_jump(int file_size, int sample_len, vector jumps) {
    int jump;

    while (1) {
        jump = random() % (file_size - (sample_len - 1));
        int status = check_repeated(jumps, jump);
        if (status == 0) {
            return jump;
        } else if (status == -1) {
            free(jumps.elems);
            exit(EXIT_FAILURE);
        }
    }

    return -1;
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "%s: missing arguments", argv[0]);
        exit(EXIT_FAILURE);
    }

    // seed para o random
    srandom(0);
    
    FILE *fp;
    char *filename = argv[1];
    int n_samples = atoi(argv[2]);

    // verificações
    if (n_samples < 0) {
        fprintf(stderr, "%s: expected at least 1 sample, %d given", argv[0], n_samples);
        exit(EXIT_FAILURE);
    }

    int sample_len = atoi(argv[3]) + 1;
    if (sample_len < 0) {
        fprintf(stderr, "%s: expected at least 1 sample, %d given", argv[0], n_samples);
        exit(EXIT_FAILURE);
    }

    if ((fp = fopen(filename, "r")) == NULL) {
        fprintf(stderr, "Error: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // tamanho do ficheiro
    int file_size = get_file_size(filename);
    if (file_size == -1) {
        exit(EXIT_FAILURE);
    } else if (file_size < sample_len) {
        fprintf(stderr, "%s: sample length can't be bigger than file size, "
        "defaulting sample length to file size and number of samples to 1\n\n", argv[0]);
        sample_len = file_size;
        n_samples = 1;
    } else {
        int possible_samples = file_size - sample_len + 1;
        if (n_samples > possible_samples) {
            fprintf(stderr, "%s: %d samples aren't possible, searching for %d instead\n\n", argv[0], 
                n_samples, possible_samples);
            n_samples = possible_samples;
        }
    }

    // criação do vetor jumps para guardar todos os saltos
    int jump;
    vector jumps;
    jumps.elems = malloc(sizeof(jumps.elems) * n_samples);
    jumps.size = 0;
    if (jumps.elems == NULL) {
        fprintf(stderr, "Error: couldn't allocate memory: malloc");
        exit(EXIT_FAILURE);
    }

    // alocação de memória para cada linha do ficheiro
    // com base em sample_len
    char *line = malloc(sizeof(char) * sample_len);

    for (int i = 0; i < n_samples; i++) {
        if (sample_len == file_size) {
            jump = 0;
        } else {
            jump = get_jump(file_size, sample_len, jumps);
        }
    
        fseek(fp, jump, SEEK_SET);
        fgets(line, sample_len, fp);
        while (1) {
            int pos = check_unwanted(line);
            if (pos == -1) {
                break;
            }

            if (pos == -2) {
                free(jumps.elems);
                fprintf(stderr, "Error: invalid line");
                exit(EXIT_FAILURE);
            }

            char *next_line = malloc(sizeof(char) * (sample_len + 1));
            fgets(next_line, sample_len - (pos + 1), fp);
            strcat(line, next_line);

            // free da variável local
            free(next_line);
        }
        
        printf(">%s<\n", line);

        jumps.elems[i] = jump;
        jumps.size++;
    }

    // free da memória dinamica
    free(line);
    free(jumps.elems);
    fclose(fp);
    exit(EXIT_SUCCESS);
}
