/*
    Rozpatrzenie pierwszego przypadku wejścia (hex).
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#include "hex.h"
#include "kolejka.h"
#include "pomocnicze.h"
#include "bitset.h"

static Kolejka *kolejkaHex;
static char *odwHex = NULL;

extern void zwolnijHex() {
    free(odwHex);
    usunKolejka(kolejkaHex);
}

extern liniaZnaki stworzLiniaZnaki(size_t rozm, char *znaki) {
    liniaZnaki z;
    z.rozm = rozm;
    z.znaki = znaki;

    return z;
}

// Zamiana tablicy charów strHex.liczby z oryginalnymi znakami wejścia
// na znaki o kodach z przedziału 0-15.
static char *naLiczby(liniaZnaki strHex) {
    for (size_t i = 0; i < strHex.rozm; ++i) {
        if ('0' <= strHex.znaki[i] && strHex.znaki[i] <= '9') {
            strHex.znaki[i] -= '0';
        }
        else if ('a' <= strHex.znaki[i] && strHex.znaki[i] <= 'f') {
            strHex.znaki[i] -= 'a' - 10;
        }
        else {
            strHex.znaki[i] -= 'A' - 10;
        }
    }

    return strHex.znaki;
}

// Zignorowanie początkowych znaków białych,
// początkowego "0x" oraz zer wiodących.
static void odrzucPoczatek(char **wsk) {
    while (isspace(**wsk)) {
        (*wsk) += sizeof(char);
    }

    (*wsk) += 2 * sizeof(char);

    while ((**wsk) == '0') {
        (*wsk) += sizeof(char);
    }
}

// Zwrócenie właściwej liczby w zapisie heksadecymalnym.
static void wezHex(liniaZnaki *wynik, char **wsk) {
    size_t rozmWynik = 4;

    while (!isspace(**wsk) && **wsk != '\0') {
        if (rozmWynik == wynik->rozm) {
            rozmWynik *= 2;
            wynik->znaki = safeRealloc(wynik->znaki,
                                       rozmWynik * sizeof(char));
        }

        if (isxdigit(**wsk)) {
            wynik->znaki[wynik->rozm] = (**wsk);
            wynik->rozm++;
        }
        else {
            wypiszBlad(4);
        }
        
        (*wsk) += sizeof(char);
    }
}

// Odrzucenie końcowych znaków białych oraz
// sprawdzenie, czy nie występują po nich znaki niebiałe.
static void odrzucKoniec(char **wsk) {
    while (isspace(**wsk)) {
        (*wsk) += sizeof(char);
    }

    if ((**wsk) != '\0' && (**wsk) != '\n') {
        wypiszBlad(4);
    }
}

// Sprawdzenie, czy długość otrzymanej liczby heksadecymalnej
// jest kompatybilna z rozmiarem labiryntu.
// wynik - liczba w zapisie heksadecymalnym wraz ze swoją długością
// maks - rozmiar labiryntu
static void sprawdzDlugosc(liniaZnaki *wynik, size_t maks) {
    size_t pierwszyZapalony = 1;
    if (wynik->rozm > 0) {
        int doBin = wynik->znaki[0];
        while (doBin % 2 != 1) {
            doBin >>= 1;
            pierwszyZapalony++;
        }
    }

    if (wynik->rozm * 4 + pierwszyZapalony > maks + 4) {
        wypiszBlad(4);
    }
}

// Zwrócenie liczby w zapisie heksadecymalnym wraz z jej długością.
extern void zwrocHex(liniaZnaki *wynik, char *sciany, size_t maks) {
    wynik->znaki = (char*) safeCalloc(4, sizeof(char));

    char *wsk = sciany;
    
    odrzucPoczatek(&wsk);

    wezHex(wynik, &wsk);

    odrzucKoniec(&wsk);

    wynik->znaki = naLiczby(*wynik);

    sprawdzDlugosc(wynik, maks);
}

// Sprawdzenie na podstawie strHex (liczba w zapisie heksydecymalnym
// i jej długość) czy na indeksie hash znajduje się ściana.
static bool czySciana(size_t hash, liniaZnaki strHex) {
    // Jeżeli hash jest większy niż liczba bitów,
    // to jest on zakodowany na zerze wiodącym (wartość false).
    if (hash >= strHex.rozm * 4) {
        return false;
    }

    // Rozpatrzenie hasha "przeciwnego" do aktualnego, ponieważ
    // pierwszy element strHex.liczby koduje 4 bity o najwiekszych indeksach
    // (odwrotnie niż zostało to podane).
    hash = 4 * strHex.rozm - hash - 1;

    // Obliczenie w której komórce strHex.liczby znajduje się zmieniony hash.
    int wynik = strHex.znaki[hash / 4];

    // Obliczenie przy użyciu przesunięcia bitowegowartości bitu,
    // który znajduje się dokładnie na indeksie hash.
    wynik >>= (3 - (hash % 4));

    return ((wynik & 1) == 0) ? (false) : (true);
}

// Sprawdzenie czy ruch z wierzchołka w.hash do aktualny jest legalny
// w.hash - indeks ojca
// w.glebokosc - głębokość ojca
// aktualny - indeks syna
// kolejkaHex - kolejka przeszukiwania grafu
// przestrzenK - przestrzenK, w której aktualnie wykonujemy ruch
// maks - maksymalny rozmiar labiryntu
// strHex - wejściowa liczba w zapisie heksadecymalnym wraz ze swoją długością
// odwHex - tablica odwiedzonych
static void sprawdzRuch(Wierzcholek w, long long aktualny, Kolejka *kolejkaHex,
                        size_t przestrzenK, size_t maks,
                        liniaZnaki strHex, char *odwHex) {

    if (0 <= aktualny && (size_t) aktualny < maks
    && !czyZajete(aktualny, odwHex) && !czySciana(aktualny, strHex)) {

        // Dzielenie daje nam numer przestrzeni k-wymiarowej,
        // w której się znajdujemy, a więc jeśli jest inny,
        // to zmieniliśmy przestrzeń, a ruch z założenia ma odbyć się wewnątrz takowej.
        if(w.hash / przestrzenK == aktualny / przestrzenK) {
            Wierzcholek v;
            v.hash = aktualny;
            v.glebokosc = w.glebokosc + 1;

            ustawPrawda(aktualny, &odwHex);
            dodajKolejka(v, kolejkaHex);
        }
    }
}

// Przeszukiwanie grafu wszerz - algorytm znalezienia najkrótszej ścieżki
// z wierzchołka o indeksie hashPocz do hashKon.
// Zwraca długość tej ścieżki lub -1 jeśli taka ścieżka nie istnieje
//
// hashPocz - indeks wierzcholka początkowego
// hashKon - indeks wierzcholka koncowego
// iloczPrefiks - struktura przydatna do określania rozmiarów kolejnych
//                k-wymiarowych przestrzeni
// k - liczba wymiarów
// strHex - wejściowa liczba w zapisie heksadecymalnym wraz ze swoją długością
extern size_t bfsHex(size_t hashPocz, size_t hashKon, size_t *iloczPrefiks,
                     size_t k, liniaZnaki strHex) {
    kolejkaHex = stworzKolejka();

    if (czySciana(hashPocz, strHex)) {
        wypiszBlad(2);
    }

    if (czySciana(hashKon, strHex)) {
        wypiszBlad(3);
    }

    Wierzcholek el;
    el.hash = hashPocz;
    el.glebokosc = 0;

    dodajKolejka(el, kolejkaHex);
    odwHex = (char*) safeCalloc(1 + (iloczPrefiks[k] / 8), sizeof(char));
    ustawPrawda(hashPocz, &odwHex);

    while (!czyPusta(kolejkaHex) && goraKolejka(kolejkaHex).hash != hashKon) {
        Wierzcholek w = wezKolejka(kolejkaHex);

        for (size_t i = 0; i < k; ++i) {
            sprawdzRuch(w, w.hash + iloczPrefiks[i], kolejkaHex,
                        iloczPrefiks[i + 1], iloczPrefiks[k], strHex, odwHex);

            sprawdzRuch(w, w.hash - iloczPrefiks[i], kolejkaHex,
                        iloczPrefiks[i + 1], iloczPrefiks[k], strHex, odwHex);
        }
    }

    size_t wynik = goraKolejka(kolejkaHex).glebokosc;
    return wynik;
}