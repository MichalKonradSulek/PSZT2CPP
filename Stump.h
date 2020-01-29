//
// Created by michalsulek on 29.01.2020.
//

#ifndef PSZT2_STUMP_H
#define PSZT2_STUMP_H

#include <algorithm>
#include <cmath>

#include "miscellaneous.h"

/** \file Stump.h
 * Plik zawierający klasy związane z drzewem decyzyjnym.
 */

/** \class DecisionStump
 * Klasa realizuje działanie drzewa decyzyjnego o głębokości 1.
 * Podstawową funkcją klasy jest metoda predict, która bada, czy określona cecha osobnika ma wartość wiąkszą czy mniejszą
 * od wartości granicznej. Następnie, jeśli wartość jest większa zwraca wynik, który powinien być zwrócony dla takiego przypadku
 * podobnie gdy wartość jest mniejsza od wartości granicznej.
 * Zwracana wartość jest postaci true lub false, co oznacza występowanie cechy wynikowej lub jej niewystępowanie.
 * Wartość ta może być następnie dekodowana przez funkcję boolToFeature.
 */
class DecisionStump {
public:
    DecisionStump() = default;
    void setAttributes(size_t indexOfDecisiveFeature, double dividingValue, bool resultIfBiggerValue, bool resultIfSmallerValue); ///<metoda ustalająca parametry drzewa: indeks cechy względem której podejmowana jest decyzja, wartość graniczna, wartość zwracana, gdy cecha jest większa od wartości granicznej, wartość zwracana, gdy cecha jest mniejsza od wartości granicznej
    bool predict(const RecordWithoutResult& sample) const; ///<metoda zwracająca predykcję, akceptuje również osobnika z cechą wynikową (RecordWithResult) wówczas cecha wynikowa jest ignorowana
private:
    size_t indexOfDecisiveFeature_;
    double dividingValue_;
    bool resultIfBiggerValue_;
    bool resultIfSmallerValue_;
};

/** \class StumpCreator
 * Klasa tworząca drzewo decyzyjne.
 * Klasa tworzy drzewo decyzyjne dla wybranej cechy, względem której będzie ono podejmować decyzję
 * W tym celu wykonywana jest następująca sekwencja czynności:
 * 1. tworzona jest tablica z przykładami, zawierająca tylko dwie kolumny: żądaną cechę, względem której drzewo będzie podejmować decyzje oraz cechę szukaną
 * 2. tablica jest sortowana
 * 3. dla każdej pary sąsiednich wierszy liczona jest wartość średnia cechy decyzyjnej
 * 4. dla każdej wartości średniej tworzone jest drzewo decyzyjne
 * 5. Każde drzewo analizuje wszystkie dostępne przykłady i badana jest jego skuteczność
 * 6. liczony jest współczynnik Giniego dla każdego drzewa
 * 7. wybierane jest drzewo z najmniejszym współczynnikiem Giniego i w ten sposób ustalana jest wartość graniczna dla drzewa
 * 8. gotowe drzewo wpisywane jest do zmiennej stump
 */
struct StumpCreator {
    StumpCreator(const Samples& samples, size_t indexOfDecisiveFeature); ///<konstruktor, jednocześnie przeprowadza on całą operację tworzeni drzewa
    std::vector<bool> tableOfCorrectClassification(const Samples& samples); ///<metoda poddaje przykłady zawarte w "samples" analizie drzewa i zwraca wektor zmiennych typu bool, określający, czy dany przypadek został sklasyfikowany prawidłowo

    DecisionStump stump; ///<faktyczne drzewo decyzyjne
    double giniImputiryOfStump; ///<współczynnik Giniego dla tego drzewa
private:
    double rateDecisionStump(const Samples& samples, const DecisionStump& tempStump); ///<metoda przeprowadzająca analizę wszystkich przypadków z "samles" i zwraca współczynnik Giniego dla danego drzewa

    bool lastStumpReturnIfTrue_; ///<zmienna przechowująca informację o tym, jaką wartość powinno zwracać ostatnio badane drzewo, gdy wartość cechy jest większa od wartości decyzyjnej
    bool lastStumpReturnIfFalse_; ///<zmienna przechowująca informację o tym, jaką wartość powinno zwracać ostatnio badane drzewo, gdy wartość cechy jest mniejsza od wartości decyzyjnej
};

bool operator < (const StumpCreator& s1, const StumpCreator& s2); ///<operator wymagany przez funkcję min_element, stwierdza, że jeden kreator jest mniejszy od drugiego, gdy zapisany w nim współczynnik Giniego jest mniejszy

#endif //PSZT2_STUMP_H
