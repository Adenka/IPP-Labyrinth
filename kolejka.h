#ifndef KOLEJKA_H
#define KOLEJKA_H

typedef struct Wierzcholek {
    size_t hash;
    long long glebokosc;
} Wierzcholek;

typedef struct Element {
    struct Wierzcholek wartosc;
    struct Element *nastepny;
} Element;

typedef struct Kolejka {
    struct Element *przod;
    struct Element *tyl;
} Kolejka;

Kolejka* stworzKolejka(void);

bool czyPusta(Kolejka *k);

void dodajKolejka(Wierzcholek war, Kolejka* k);

Wierzcholek wezKolejka(Kolejka* k);

Wierzcholek goraKolejka(Kolejka *k);

void usunKolejka(Kolejka *k);

#endif /* KOLEJKA_H */