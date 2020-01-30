//
// Created by michalsulek on 29.01.2020.
//

#ifndef PSZT2_ADABOOSTALGORITHM_H
#define PSZT2_ADABOOSTALGORITHM_H

#include "Stump.h"

/** \file Stump.h
 * Plik zawierający klasę z algorytmem AdaBoost.
 */

/** \class AdaBoostAlgorithm
 * Klasa realizuje działanie algorytmu AdaBoost opartego na drzewach decyzyjnych o głębokości 1.
 * Wybór drzew dokonywany jest na podstawie minimalizacji ważonych ilości błędnych decyzji. Każde kolejne krzewo tworzone jest
 * z uwzględnieniem błędów popełnionych przez drzewo poprzednie. W tym celu każdemu przykładowi, wykorzystywanemu do
 * trenowania algorytmu przypisywana jest waga, która jest zwiększana, gdy przykład został sklasyfikowany źle
 * i zmniejszana, gdy został sklasyfikowany dobrze. Następnie, przy wyborze kolejnego drzewa, przypadki z większą
 * wagą mają większy wpływ na jego kształt
 */
class AdaBoostAlgorithm {
private:
    StumpCreator createStump(const Samples& samples, const std::vector<double>& weightsOfSamples); ///<metoda tworząca kolejne drzewo decyzyjne, zwracająca kreatora tego drzewa. Drzewo jest tworzone w następujący sposób: dla każdej cechy tworzone jest drzewo, a następnie wybierane takie o najmniejszym współczynniku Giniego
    double calculateAmountOfSay(const std::vector<double>& weightsOfSamples, const std::vector<bool>& tableOfCorrectClassification); ///<metoda licząca wpływ drzewa na ostateczną decyzję algorytmu (wagę drzewa), na podstawie ilości podejmowanych poprawnych decyzji
    void recalculateWeights(std::vector<double>& weightsOfSamples, const std::vector<bool>& tableOfCorrectClassification, double amountOfSay); ///<metoda aktualizując wagi przykładów
    void normalizeWeights(std::vector<double>& weightsOfSamples); ///<metoda ormalizująca wagi przykładów tak, by ich suma była równa 1

    std::vector<DecisionStump> stumps_; ///<drzewa decyzyjne
    std::vector<double> amountOfSay_; ///<wagi drzew
    double dividingValueOfPredictedAttribute_ = 0; ///<zmienna przechhowująca liczbę, dzielącą szukaną cechę na dwie klasy
public:
    void trainAlgorithm(const Samples& samples, size_t numberOfStumps, double dividingValueOfPredictedAttribute); ///<metoda trenująca algorytm
    double prediction(const RecordWithoutResult& record); ///<metoda zwracająca predykcję algorytmu
};


#endif //PSZT2_ADABOOSTALGORITHM_H
