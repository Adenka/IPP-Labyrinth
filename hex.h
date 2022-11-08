#ifndef HEX_H
#define HEX_H

void zwolnijHex();

typedef struct liniaZnaki {
    size_t rozm;
    char *znaki;
} liniaZnaki;

liniaZnaki stworzLiniaZnaki(size_t rozm, char* znaki);

char *maleLitery(liniaZnaki strHex);

void zwrocHex(liniaZnaki *wynik, char *wsk, size_t maks);

size_t bfsHex(size_t hashPocz, size_t hashKon, size_t *iloczPrefiks, size_t k, liniaZnaki strHex);

#endif /* HEX_H */