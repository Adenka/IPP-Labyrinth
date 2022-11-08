/*
    Rozpatrzenie drugiego przypadku wejścia (R).
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <limits.h>

#include "bitset.h"
#include "ciag.h"
#include "kolejka.h"
#include "pomocnicze.h"

static Kolejka *kolejkaCiag;
static char *odwISciany = NULL;

extern void zwolnijCiag() {
    usunKolejka(kolejkaCiag);
    free(odwISciany);
}

// Zamiana fragmentu napisu na liczbę całkowitą.
// Następuje modyfikacja oryginalnego wskaźnika podanego w argumentach.
// aktualnaCyfra - wskaźnik do wskaźnika miejsca, gdzie wczytano
//                 pierwszy niebiały znak
//                 przesuwamy go dopóki napotykamy cyfry
static long long doInt32(char **aktualnaCyfra) {
    size_t wynik = 0;

    // Zliczenie liczby cyfr w wyniku.
    int licznik = 0;

    while (!isspace(**aktualnaCyfra)) {
        if (licznik > 10) {
            wypiszBlad(4);
        }

        if ('0' <= **aktualnaCyfra && **aktualnaCyfra <= '9') {
            wynik *= 10;
            wynik += **aktualnaCyfra - '0';
        }
        else {
            wypiszBlad(4);
        }
        *aktualnaCyfra += sizeof(char);
        ++licznik;
    }

    if (wynik > UINT_MAX) {
        wypiszBlad(4);
    }

    return wynik;
}

// Zwrócenie pięciu liczb odpowiedzialnych za generowanie ścian.
// sciany - wczytana czwarta linia wejścia
extern generatorCiagu zwrocGenerator(char *sciany) {
    size_t ciag[5];

    // Zmienna określająca ile liczb w napisie już znaleźliśmy,
    // na końcu działania funkcji powinna przyjąć wartość 5
    size_t wczytanoR = 0;
    bool zaczete = false;

    for (char *wsk = sciany; *wsk != '\0'; wsk += sizeof(char)) {
        if (!isspace(*wsk)) {
            if (zaczete) {
                if (wczytanoR >= 5) {
                    wypiszBlad(4);
                }

                ciag[wczytanoR++] = doInt32(&wsk);

            }
            else {
                zaczete = true;
            }
        }
    }

    if (wczytanoR < 5) {
        wypiszBlad(4);
    }

    generatorCiagu wynik;
    wynik.a = ciag[0];
    wynik.b = ciag[1];
    wynik.m = ciag[2];
    if (wynik.m == 0) {
        wypiszBlad(4);
    }
    wynik.r = ciag[3];
    wynik.s0 = ciag[4];

    return wynik;
}

// Wygenerowanie i zapisanie wszystkich ścian.
// odwISciany - wskaźnik do kolekcji wartości zerojedynkowych,
//              na którym pamiętane będą wygenerowane ściany
// ciag - pięć liczb, na podstawie których generowany jest ciąg
// maks - maksymalny rozmiar labiryntu
static void stablicujSciany(char **odwISciany, generatorCiagu ciag, size_t maks) {
    *odwISciany = (char*) safeCalloc(1 + maks / 8, sizeof(char));
    size_t wyraz = ciag.s0;
    for (size_t i = 0; i<ciag.r; ++i) {
        wyraz *= ciag.a;
        wyraz %= ciag.m;
        wyraz += ciag.b;
        wyraz %= ciag.m; 

        // Wygenerowanie wszystkich ścian,
        // które przystają do zmiennej wyraz przy modulo 2^32.
        size_t kopia = wyraz % maks;
        while (kopia < maks){
            ustawPrawda(kopia, odwISciany);
            kopia += ((size_t) 1) << 32;
        }
    }
}

// Sprawdzenie czy ruch z wierzchołka w.hash do aktualny jest legalny
// w.hash - indeks ojca
// w.glebokosc - głębokość ojca
// aktualny - indeks syna
// kolejkaCiag - kolejka przeszukiwania grafu
// przestrzenK - przestrzenK, w której aktualnie wykonujemy ruch
// maks - maksymalny rozmiar labiryntu
// odwISciany - tablica odwiedzonych oraz ścian (komórek labirytu, do których)
//              nie możemy wejsć
static void sprawdzRuch(Wierzcholek w, long long aktualny,
                        Kolejka *kolejkaCiag, size_t przestrzenK,
                        size_t maks, char *odwISciany) {

    if (0 <= aktualny && (size_t) aktualny < maks
    && !czyZajete(aktualny, odwISciany)) {

        // Dzielenie określa numer aktualnej przestrzeni k-wymiarowej -
        // jeśli wyniki dzielenia są inne, to podczas ruchu zmieniliśmy przestrzeń,
        // a ruch z założenia ma odbyć się wewnątrz jednej przestrzeni
        if (w.hash / przestrzenK == aktualny / przestrzenK) {
            Wierzcholek v;
            v.hash = aktualny;
            v.glebokosc = w.glebokosc + 1;

            ustawPrawda(aktualny, &odwISciany);
            dodajKolejka(v, kolejkaCiag);
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
// ciag - pięć liczb, na podstawie których generowany jest ciąg.
extern size_t bfsCiag(size_t hashPocz, size_t hashKon,
                      size_t *iloczPrefiks, size_t k, generatorCiagu ciag) {
    kolejkaCiag = stworzKolejka();

    stablicujSciany(&odwISciany, ciag, iloczPrefiks[k]);

    if (czyZajete(hashPocz, odwISciany)) {
        wypiszBlad(2);
    }

    if (czyZajete(hashKon, odwISciany)) {
        wypiszBlad(3);
    }
    
    Wierzcholek el;
    el.hash = hashPocz;
    el.glebokosc = 0;

    dodajKolejka(el, kolejkaCiag);
    ustawPrawda(hashPocz, &odwISciany);

    while (!czyPusta(kolejkaCiag) && goraKolejka(kolejkaCiag).hash != hashKon) {
        Wierzcholek w = wezKolejka(kolejkaCiag);

        for (size_t i = 0; i < k; ++i) {
            sprawdzRuch(w, w.hash + iloczPrefiks[i], kolejkaCiag,
                        iloczPrefiks[i + 1], iloczPrefiks[k], odwISciany);

            sprawdzRuch(w, w.hash - iloczPrefiks[i], kolejkaCiag,
                        iloczPrefiks[i + 1], iloczPrefiks[k], odwISciany);
        }
    }

    size_t wynik = goraKolejka(kolejkaCiag).glebokosc;
    return wynik;
}   