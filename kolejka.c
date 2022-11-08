/*
    Implementacja kolejki wskaźnikowej.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "kolejka.h"
#include "pomocnicze.h"

extern Kolejka *stworzKolejka() {
    Kolejka *k = safeMalloc(sizeof(Kolejka));
    k->przod = NULL;
    k->tyl = NULL;

    return k;
}

extern bool czyPusta(Kolejka *k) {
    if (k == NULL) return true;
    return (k->przod == NULL);
}

extern void dodajKolejka(Wierzcholek war, Kolejka *k) {
    Element *tmp = (Element*) safeMalloc(sizeof(Element));

    tmp->wartosc = war;
    tmp->nastepny = NULL;
    if (!czyPusta(k)) {
        k->tyl->nastepny = tmp;
        k->tyl = tmp;
    }
    else {
        k->przod = tmp;
        k->tyl = tmp;
    }
}

// Usunięcie pierwszego elementu z kolejki i zwrócenie jego wartości.
extern Wierzcholek wezKolejka(Kolejka *k) {
    Wierzcholek war;
    war.hash = -1;
    war.glebokosc = -1;

    if (!czyPusta(k)) {
        Element *tmp = k->przod;

        war = k->przod->wartosc;
        k->przod = k->przod->nastepny;
        free(tmp);
    }

    return war;
}

// Zwrócenie wartości pierwszego elementu kolejki bez jego usuwania.
extern Wierzcholek goraKolejka(Kolejka *k) {
    Wierzcholek war;
    war.hash = -1;
    war.glebokosc = -1;

    if (!czyPusta(k)) {
        war = k->przod->wartosc;
    }

    return war;
}

// Zwolnienie całej pamięci zajmowanej przez kolejkę.
void usunKolejka(Kolejka *k) {
    while (!czyPusta(k)) {
        Element *tmp = k->przod;
        k->przod = k->przod->nastepny;
        free(tmp);
    }

    free(k);
}