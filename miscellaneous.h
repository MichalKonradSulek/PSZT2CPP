//
// Created by michalsulek on 29.01.2020.
//

#ifndef PSZT2_MISCELLANEOUS_H
#define PSZT2_MISCELLANEOUS_H

#include <iostream>
#include <vector>

/** \file miscellaneous.h
 * Plik zawierający dodatkowe funkcje i definicje niezbędne do działania algorytmu.
 */

typedef std::vector<double> RecordWithResult; ///<kontener na jeden obiekt cech z wynikiem; szukana cecha jest na końcu
typedef std::vector<double> RecordWithoutResult; ///<kontener na jeden obiekt cech bez wyniku
typedef std::vector<RecordWithResult> Samples; ///<kontener z próbkami

bool operator < (const RecordWithResult& r1, const RecordWithResult& r2); ///<operator wymagany przez funkcję std::sort

bool featureToBool(double feature,  double dividingValue); ///<funkcja zmieniajaca wartość liczbową (cechę) na operator boolowski używany przez drzewo decyzyjne

double boolToFeature(bool b,  double dividingValue); ///<funkcja zamieniająca operator boolowski używany przez drzewo decyzyjne na wartość liczbową (cechę)

Samples readSamples(const std::string& file, size_t nOfColumns); ///<wczytuje dane z pliku i umieszcza w strukturze Samples, argumenty: nazwa pliku, liczba kolumn

void showRecord(const RecordWithResult& record, double weight); ///<wyświetla pojedynczy rekord (osobnika)

void showSamples(const Samples& samples, const std::vector<double>& weights); ///<wyświetla zawartość tablicy z osobnikami

void moveBackColumn(Samples& samples, size_t columnIndex); ///<funkcja przestawia wybraną kolumnę na koniec zestawu danych, argumenty: tablica z danymi, kolumna do przesunięcia

#endif //PSZT2_MISCELLANEOUS_H
