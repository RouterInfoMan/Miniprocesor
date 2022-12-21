#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DIM_SIZE 4
#define N_SIZE 3
#define UINT32_SIZE 32
#define UINT16_SIZE 16

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

typedef struct {
    __uint16_t buf;
    int pos;
}uint16parser;

void init_parser(uint16parser *par) {
    par->pos = -1;
}
void read_dim(uint16parser *par, __uint16_t dim, int *out) {
    if (par->pos < 0) {
        par->pos = UINT16_SIZE - 1;
        scanf("%hu", &par->buf);
    }
    if (dim - 1 <= par->pos) {
        *out = (int)getNtoMnum(par->buf, par->pos - dim + 1, par->pos);
        par->pos -= dim;
    } else {
        __uint16_t remainder = dim - par->pos - 1;
        *out = (int) getNtoMnum(par->buf, 0, par->pos);
        (*out) <<= remainder;
        int tmp = 0;
        par->pos -= dim;
        read_dim(par, remainder, &tmp);
        *out += tmp;
    }
}
int solve_prods(int *opds, char *opts, int *np, int *op, int len) {
    int res = opds[*np];
    (*np)++;
    while (*op < len && (opts[*op] == '*' || opts[*op] == '/')) {
        if (opts[*op] == '*') {
            res *= opds[*np];
            (*np)++;
        } else {
            res /= opds[*np];
            (*np)++;
        }
        (*op)++;
    }
    return res;
}
int solve_sums(int *opds, char *opts, int *np, int *op, int len) {
    int res = solve_prods(opds, opts, np, op, len);
    while (*op < len && (opts[*op] == '+' || opts[*op] == '-')) {
        if (opts[*op] == '+') {
            (*op)++;
            res += solve_prods(opds, opts, np, op, len);
        } else {
            (*op)++;
            res -= solve_prods(opds, opts, np, op, len);
        }
    }
    return res;
}
void exec_instruction(instruction *mi) {
    unsigned int bits = mi->dim * (mi->N + 1) / UINT16_SIZE;
    unsigned int shorts = bits / UINT16_SIZE
                        + (bits % UINT16_SIZE != 0 ? 1 : 0);

    uint16parser *par = (uint16parser *) malloc(sizeof(uint16parser));
    init_parser(par);

    int *tmp = (int *) malloc((mi->N + 1) * sizeof(int));
    for (int i = 0; i < mi->N + 1; i++) {
        read_dim(par, mi->dim, &tmp[i]);
    }

    int np = 0, op = 0;
    int res = solve_sums(tmp, mi->ops, &np, &op, (int) mi->N);
    printf("%d\n", res);
    free(par);
}
int main() {
    __uint32_t cod = 0;
    scanf("%u", &cod);

    instruction *mi = decod(cod);
    exec_instruction(mi);

    free(mi->ops);
    free(mi);
}
