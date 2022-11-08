/*
    Funkcje niezbędne do wczytania wejścia,
    zamiany jego trzech pierwszych linii na liczby
    oraz inne funkcje pomocnicze.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <ctype.h>

#include "ciag.h"
#include "hex.h"
#include "pomocnicze.h"
#include "wejscie.h"

// Wskaźniki globalne na NULL - brak problemu przy wywoływaniu free.
// 1sza, 2ga i 3cia linia będą od razu przerabiane na liczby.
// Wskaźnik do czwartej linii zadeklarowany jest w mainie,
// bo tam wywołamy odpowiednie funkcje do wyciągnięcia z niego danych.
static char *pierwszaLinia = NULL;
static char *drugaLinia = NULL;
static char *trzeciaLinia = NULL;
static char *piataLinia = NULL;

extern void zwolnijWejscie() {
    free(pierwszaLinia);
    free(drugaLinia);
    free(trzeciaLinia);
    free(piataLinia);
}

extern liniaLiczby stworzLiniaLiczby(size_t rozm, size_t *liczby) {
    liniaLiczby l;
    l.rozm = rozm;
    l.liczby = liczby;

    return l;
}

static void wczytajLinie(char **linia, size_t numerLinii) {
    // Jeżeli 1, 2, 3cia linia nie istnieją, wypisz błąd.
    size_t liniaRozm = 0;
    if (getline(linia, &liniaRozm, stdin) < 0) {
        wypiszBlad(numerLinii);
    }
}

// Sprawdzenie kompatybilności rozmiarów poszczególnych linii wejścia.
static void czyKompatybilne(liniaLiczby wym, liniaLiczby por, size_t numerLinii) {
    if (wym.rozm != por.rozm) {
        wypiszBlad(numerLinii);
    }

    size_t k = wym.rozm;
    for (size_t i = 0; i < k; ++i) {
        if (por.liczby[i] > wym.liczby[i]) {
            wypiszBlad(numerLinii);
        }
    }
}

// Zamiana fragmentu napisu na dużą liczbę całkowitą.
// Następuje modyfikacja oryginalnego wskaźnika podanego w argumentach.
// aktualnaCyfra - wskaźnik do wskaźnika miejsca, gdzie wczytano
//                 pierwszy niebiały znak
// numerLinii - numer linii, która jest rozpatrywana
//              (błąd której linii należy ewentualnie wypisać)
static size_t doULLInt(char **aktualnaCyfra, size_t numerLinii) {
    size_t wynik = 0;

    // Zliczanie cyfr aktualnego wyniku.
    int licznik = 0;
    while (!isspace(**aktualnaCyfra)) {
        if (licznik > 20) {
            wypiszBlad(numerLinii);
        }
        
        if ('0' <= **aktualnaCyfra && **aktualnaCyfra <= '9') {
            size_t a = **aktualnaCyfra - '0';

            if ((__SIZE_MAX__ - a) / 10 < wynik) { 
                wypiszBlad(numerLinii);
            }

            wynik *= 10;
            wynik += **aktualnaCyfra - '0';
        }
        else {
            wypiszBlad(numerLinii);
        }
        *aktualnaCyfra += sizeof(char);
        ++licznik;
    }

    if (wynik == 0) {
        wypiszBlad(numerLinii);
    }

    return wynik;
}

// Podział wczytanego do tablicy znaków wejścia (linia) na liczby.
static void rozdzielWejscieLiczby(liniaLiczby *wynik, char *linia,
                                  size_t numerLinii) {
    size_t rozmWynik = 4;
    wynik->liczby = (size_t*) safeCalloc(rozmWynik, sizeof(size_t));

    for (char *wsk = linia; *wsk != '\0'; wsk += sizeof(char)) {
        if ('0' <= *wsk && *wsk <= '9') {
            if (rozmWynik == wynik->rozm) {
                rozmWynik *= 2;
                wynik->liczby = safeRealloc(wynik->liczby,
                                            rozmWynik * sizeof(size_t));
            }

            wynik->liczby[wynik->rozm] = doULLInt(&wsk, numerLinii);
            wynik->rozm++;
        }
    }

    if (wynik->rozm == 0) {
        wypiszBlad(numerLinii);
    }
}

// Wczytanie wejścia - przekazane zostają wskaźnik do struktur,
// do których dane mają zostać zapisane.
extern void wczytajWejscie(liniaLiczby *wym, liniaLiczby *pocz, liniaLiczby *kon,
                    char **sciany) {

    wczytajLinie(&pierwszaLinia, 1);
    rozdzielWejscieLiczby(wym, pierwszaLinia, 1);

    wczytajLinie(&drugaLinia, 2);
    rozdzielWejscieLiczby(pocz, drugaLinia, 2);

    czyKompatybilne(*wym, *pocz, 2);

    wczytajLinie(&trzeciaLinia, 3);
    rozdzielWejscieLiczby(kon, trzeciaLinia, 3);

    czyKompatybilne(*wym, *kon, 3);

    wczytajLinie(sciany, 4);
}

// Określenie typu wejścia na podstawie 4tej linii wejścia.
extern bool ktoreWejscie(char *sciany) {
    bool czyZnalezione = false;
    int wynik = -1;
    for (char *wsk = sciany; !czyZnalezione && (*wsk) != '\0'; wsk += sizeof(char)) {
        if (!isspace(*wsk)) {
            if (*wsk == '0' && *(wsk + sizeof(char)) == 'x') {
                wynik = 0;
                czyZnalezione = true;
            }
            else if (*wsk == 'R') {
                wynik = 1;
                czyZnalezione = true;
            }
            else {
                wypiszBlad(4);
            }
        }
    }

    if (wynik == -1) {
        wypiszBlad(4);
    }

    return wynik;
}

// Obliczenie indeksu (hasha) dowolnej komórki labiryntu.
extern size_t hash(size_t *wspolrzedne, size_t *iloczPrefiks, size_t k, size_t numerLinii) {
    size_t wynik = 0;
    for (size_t i = 0; i < k; ++i) {
        wynik += ((wspolrzedne[i] - 1) * iloczPrefiks[i]);
    }
    
    if (wynik >= iloczPrefiks[k]) {
        wypiszBlad(numerLinii);
    }
    
    return wynik;
}

// Obliczenie iloczynów prefiksowych.
extern void prefixy(size_t **iloczPrefiks, size_t *wymiary, size_t k) {
    (*iloczPrefiks) = (size_t *) safeMalloc((k + 1) * (sizeof(size_t)));
    (*iloczPrefiks)[0] = 1;

    for (size_t i = 1; i <= k; ++i) {
        double czyZaDuze = (double)__SIZE_MAX__ / (*iloczPrefiks)[i - 1];
        if (czyZaDuze >= wymiary[i - 1]) {
            (*iloczPrefiks)[i] = (*iloczPrefiks)[i - 1] * wymiary[i - 1];
        }
        else {
            wypiszBlad(0);
        }
    }
}

//Próba wczytania piątej linii wejścia.
extern void sprawdzPiataLinia() {
    size_t wymiary5 = 0;
    if (getline(&piataLinia, &wymiary5, stdin) >= 0) {
        wypiszBlad(5);
    }
}