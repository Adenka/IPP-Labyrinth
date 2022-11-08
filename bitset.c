/*
    Implementacja kolekcji wartości zerojedynkowych.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Określenie czy bit o indeksie numer
// w kolekcji wartości zerojedynkowej bitZbior jest zapalony czy nie.
extern bool czyZajete(size_t numer, char *bitZbior) {
    // Określenie w którym znaku bitZbioru znajduje bit o indeksie numer.
    char znak = bitZbior[numer / 8];

    // Nadanie zmiennej wartości równej wartości bitu na pozycji numer.
    znak >>= (numer % 8);

    // Zwrócenie wartości logicznej za pomocą operacji logicznej AND.
    return (znak & 1);
}

// Ustawienie wartości bitu o indeksie numer
// w kolekcji wartości zerojedynkowej bitZbior na prawda.
extern void ustawPrawda(size_t numer, char **bitZbior) {
    // Określenie w którym znaku bitZbioru znajduje bit o indeksie numer.
    char znak = (*bitZbior)[numer / 8];

    // Utworzenie pomocniczej liczby, która będzie mieć wartość prawda
    // wyłącznie na pozycji o indeksie numer.
    int potega = (1 << (numer % 8));

    // Ustawienie wartości na pozycji o indeksie numer
    // za pomocą operacji logicznej OR.
    znak = znak | potega;

    // Wpisanie nowej wartosci do kolekcji wartości zerojedynkowych.
    (*bitZbior)[numer / 8] = znak;
}