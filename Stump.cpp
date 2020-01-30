//
// Created by michalsulek on 29.01.2020.
//

#include <cfloat>

#include "Stump.h"

void DecisionStump::setAttributes(size_t indexOfDecisiveFeature, double dividingValue, bool resultIfBiggerValue, bool resultIfSmallerValue) {
    indexOfDecisiveFeature_ = indexOfDecisiveFeature;
    dividingValue_ = dividingValue;
    resultIfBiggerValue_ = resultIfBiggerValue;
    resultIfSmallerValue_ = resultIfSmallerValue;
}
bool DecisionStump::predict(const RecordWithoutResult& sample) const { // also works with RecordWithResult
    if(sample.at(indexOfDecisiveFeature_) > dividingValue_) return resultIfBiggerValue_;
    else return resultIfSmallerValue_;
}

StumpCreator::StumpCreator(const Samples& samples, size_t indexOfDecisiveFeature, const std::vector<double>& weights, double dividingValueOfPredictedAttribute)
    : dividingValueOfPredictedAttribute_(dividingValueOfPredictedAttribute), lastStumpReturnIfFalse_(true),
    lastStumpReturnIfTrue_(true) {
    if (samples.empty()) throw "no samples"; // jeśli nie ma próbek, to nie stworzy się drzewa
    if (indexOfDecisiveFeature >= samples.front().size() - 1)
        throw "no features"; // jeśli nie ma cech, to nie stworzy się drzewa
    Samples samplesWithOneFeatureAndResult(samples.size(),
                                           RecordWithResult(2)); // tworzymy kontener na dane, który ma tyle wierszy, co "samples" i 2 kolumny (na cechę decyzyjną i wynikową)
    for (size_t i = 0; i < samples.size(); ++i) {
        samplesWithOneFeatureAndResult.at(i).at(0) = samples.at(i).at(indexOfDecisiveFeature); // cecha decyzyjna
        samplesWithOneFeatureAndResult.at(i).at(1) = samples.at(i).back(); // cecha szukana (wynikowa)
    }
    std::sort(samplesWithOneFeatureAndResult.begin(), samplesWithOneFeatureAndResult.end());
    double minimalStumpRate = DBL_MAX; // ustawiamy na 1 - jest to najgorszy możliwy przypadek, praktycznie nieosiągalny
    double dividingValueForMinimalSR = samplesWithOneFeatureAndResult.front().front(); // tymczasowo ustawiamy na cokolwiek - na wartość pierwszego elementu
    bool stumpReturnIfTrue = true; // w tym momencie będziemy sprawdzać, ile przypadków jest powyżej wartości granicznej, a ile poniżej, więc dobrze będzie, gdy nasze drzewo będzie zwracało po prostu true i false w zależności od podejmowanej decyzji
    bool stumpReturnIfFalse = false;
    for (size_t i = 0;
         i < samplesWithOneFeatureAndResult.size() - 1; ++i) { // - 1, gdyż liczymy średnią z dwóch sąsiednich, to jest pętla, w której badamy drzewa dla każdej wartości średniej i obliczamy dla nich współczynnik Giniego
        double dividingValue = (samplesWithOneFeatureAndResult.at(i).at(0) +
                                samplesWithOneFeatureAndResult.at(i + 1).at(0)) / 2.0;
        DecisionStump tempStump;
        tempStump.setAttributes(0, dividingValue, true, false); // 0, gdyż pod 0wym indeksem znajduje się analizowana cecha
        double stumpRate = rateDecisionStump(samplesWithOneFeatureAndResult, tempStump, weights);
        if (stumpRate < minimalStumpRate) {
            minimalStumpRate = stumpRate;
            dividingValueForMinimalSR = dividingValue;
            stumpReturnIfTrue = lastStumpReturnIfTrue_; // lastStumpReturnIfTrue_ jest liczone w metodzie rateDecisionStump
            stumpReturnIfFalse = lastStumpReturnIfFalse_;
        }
    }
    rateOfThisStump = minimalStumpRate;
    stump.setAttributes(indexOfDecisiveFeature, dividingValueForMinimalSR, stumpReturnIfTrue, stumpReturnIfFalse);
}

std::vector<bool> StumpCreator::tableOfCorrectClassification(const Samples& samples) {
    std::vector<bool> tableOfCorrectClassification(samples.size(), false); // na początku zakładamy, że wszystkieodpowiedzi są błędne
    for(size_t i = 0; i < samples.size(); ++i) {
        if(stump.predict(samples.at(i)) == featureToBool(samples.at(i).back(), dividingValueOfPredictedAttribute_))
            tableOfCorrectClassification.at(i) = true; // gdy jakaś odpowiedź jest prawidłowa, to ustawiamy tę pozycję na true
    }
    return tableOfCorrectClassification;
}

double StumpCreator::rateDecisionStump(const Samples& samples, const DecisionStump& tempStump, const std::vector<double>& weights) {
    double stumpSaidTrueAndItIs = 0; // liczba przypadków, dla których drzewo powiedziało, że prawda i faktycznie była prawda
    double stumpSaidTrueButItIsNot = 0;
    double stumpSaidFalseAndItIs = 0;
    double stumpSaidFalseButItIsNot = 0;
    for(size_t i = 0; i < samples.size(); ++i) { // pętla zliczająca odpowiedzi drzewa na oreśloną cechę
        if(tempStump.predict(samples.at(i)) == true) {
            if(featureToBool(samples.at(i).back(), dividingValueOfPredictedAttribute_) == true) {
                stumpSaidTrueAndItIs += weights.at(i);
            }
            else {
                stumpSaidTrueButItIsNot += weights.at(i);
            }
        }
        else {
            if(featureToBool(samples.at(i).back(), dividingValueOfPredictedAttribute_) == false) {
                stumpSaidFalseAndItIs += weights.at(i);
            }
            else {
                stumpSaidFalseButItIsNot += weights.at(i);
            }
        }
    }
    double valueToMinimize = 0;
    if(stumpSaidTrueAndItIs > stumpSaidFalseButItIsNot) {
        lastStumpReturnIfTrue_ = true; // tutaj decydujemy, jaką odpowiedź powinno dać drzewo, gdy wartość jest większa od decyzyjnej; ustalane jest to na podstawie tego, jakich odpowiedzi w tej gałęzi było więcej
        valueToMinimize += stumpSaidTrueButItIsNot;
    }
    else {
        lastStumpReturnIfTrue_ = false;
        valueToMinimize += stumpSaidTrueAndItIs;
    }
    if(stumpSaidFalseAndItIs > stumpSaidFalseButItIsNot) {
        lastStumpReturnIfFalse_ = false; // analogicznie dla gałęzi mniejszej od wartości decyzyjnej
        valueToMinimize += stumpSaidFalseButItIsNot;
    }
    else {
        lastStumpReturnIfFalse_ = true;
        valueToMinimize += stumpSaidFalseAndItIs;
    }
    return valueToMinimize;
}

bool operator < (const StumpCreator& s1, const StumpCreator& s2) {
    return s1.rateOfThisStump < s2.rateOfThisStump;
}