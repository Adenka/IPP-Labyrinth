#ifndef WEJSCIE_H
#define WEJSCIE_H

typedef struct liniaLiczby {
    size_t rozm;
    size_t *liczby;
} liniaLiczby;

void zwolnijWejscie();

void wczytajWejscie(liniaLiczby *wym, liniaLiczby *pocz, liniaLiczby *kon,
                    char **sciany);

liniaLiczby stworzLiniaLiczby(size_t rozm, size_t* liczby);

bool ktoreWejscie();

size_t hash(size_t *wspolrzedne, size_t *iloczPrefiks, size_t k, size_t numerLinii);

void prefixy(size_t **iloczPrefiks, size_t *wymiary, size_t k);

void sprawdzPiataLinia();

#endif /* WEJSCIE_H */