/*
    MAŁE ZADANIE - LABIRYNT

    autor: Jagoda Bobińska
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "ciag.h"
#include "hex.h"
#include "pomocnicze.h"
#include "wejscie.h"

static liniaLiczby wym;
static liniaLiczby pocz;
static liniaLiczby kon;
static char *sciany;

static size_t *iloczPrefiks = NULL;

static liniaZnaki strHex;

//Ustawienie wskaźników do części sktruktur,
//które korzystają z dynamicznej alokacji pamięci, na NULL.
static void inicjalizuj() {
    wym = stworzLiniaLiczby(0, NULL);
    pocz = stworzLiniaLiczby(0, NULL);
    kon = stworzLiniaLiczby(0, NULL);

    strHex = stworzLiniaZnaki(0, NULL);
}

static void zwolnijMain() {
    free(wym.liczby);
    free(pocz.liczby);
    free(kon.liczby);
    free(sciany);

    free(iloczPrefiks);

    free(strHex.znaki);

    zwolnijWejscie();
    zwolnijHex();
    zwolnijCiag();
}

int main() {
    atexit(zwolnijMain);
    
    inicjalizuj();
    
    wczytajWejscie(&wym, &pocz, &kon, &sciany);

    //Obliczenie iloczynów prefiksowych -
    //struktury przydatnej przy przeszukiwaniu labiryntu.
    size_t k = wym.rozm;
    prefixy(&iloczPrefiks, wym.liczby, k);

    //Obliczenie indeksu (hasha) początkowego i końcowego pola.
    size_t hashPocz = hash(pocz.liczby, iloczPrefiks, k, 2);
    size_t hashKon = hash(kon.liczby, iloczPrefiks, k, 3);

    //Wywołanie odpowiedniego wejścia i obliczenie wyniku.
    long long wynik = -1;
    if (ktoreWejscie(sciany) == 0) {
        zwrocHex(&strHex, sciany, iloczPrefiks[k]);
        wynik = bfsHex(hashPocz, hashKon, iloczPrefiks, k, strHex);
    }
    else {
        struct generatorCiagu ciag = zwrocGenerator(sciany);
        wynik = bfsCiag(hashPocz, hashKon, iloczPrefiks, k, ciag);
    }

    sprawdzPiataLinia();

    //Wypisanie wyniku obliczonego w przeszukiwaniu grafu.
    //Jeśli wynik jest równy -1, to nie udało się znaleźć ścieżki.
    if (wynik > -1) printf("%lld\n", wynik);
    else printf("NO WAY\n");
    
    return 0;
}