#include <stdio.h>
#include <stdlib.h>

#define DIM_SIZE 4
#define N_SIZE 3
#define UINT32_SIZE 32

typedef struct {
    char *ops;
    __uint32_t N;
    __uint32_t dim;
}instruction;

__uint32_t get_nth_bit(__uint32_t src, int n) {
    unsigned int mask = (1 << n);
    return ((src & mask) >> n);
}
__uint32_t getNtoMnum(__uint32_t src, int n, int m) {
    int size = m - n;
    __uint32_t mask = (1 << size) - 1;
    mask = (mask << 1) + 1;  // To prevent overflow

    mask <<= n;
    __uint32_t out = (src & mask) >> n;
    return out;
}

instruction *decod(__uint32_t cod) {
    instruction *mi = (instruction *) malloc(sizeof(instruction));

    mi->N = getNtoMnum(cod, UINT32_SIZE - N_SIZE, UINT32_SIZE - 1) + 1;

    mi->ops = (char *) malloc(mi->N * sizeof(char));
    for (int i = (UINT32_SIZE - N_SIZE) - 1, k = 0; k < mi->N; i -= 2, k++) {
        __uint32_t tmp = getNtoMnum(cod, i - 1, i);
        switch (tmp) {
            case 0:
                mi->ops[k] = '+';
                break;
            case 1:
                mi->ops[k] = '-';
                break;
            case 2:
                mi->ops[k] = '*';
                break;
            case 3:
                mi->ops[k] = '/';
                break;
        }
    }
    int bits = UINT32_SIZE - (N_SIZE + 2 * mi->N + DIM_SIZE);
    mi->dim = getNtoMnum(cod, bits, bits + DIM_SIZE - 1) + 1;
    return mi;
}

void print_instruction(instruction *mi) {
    printf("%d ", mi->N);
    for (int i = 0; i < mi->N; i++) {
        printf("%c ", mi->ops[i]);
    }
    printf("%d\n", mi->dim);
}

int main() {
    __uint32_t cod = 0;
    scanf("%u", &cod);

    instruction *mi = decod(cod);
    print_instruction(mi);

    free(mi->ops);
    free(mi);
}
