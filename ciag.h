#ifndef CIAG_H
#define CIAG_H

typedef struct generatorCiagu {
    size_t a;
    size_t b;
    size_t m;
    size_t r;
    size_t s0;
} generatorCiagu;

void zwolnijCiag();

generatorCiagu zwrocGenerator(char *sciany);

size_t bfsCiag(size_t hashPocz, size_t hashKon, size_t *iloczPrefiks, size_t k, generatorCiagu ciag);

#endif /* CIAG_H */