//
// Created by michalsulek on 29.01.2020.
//

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

StumpCreator::StumpCreator(const Samples& samples, size_t indexOfDecisiveFeature) {
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
    double minimalGiniImpurity = 1; // ustawiamy na 1 - jest to najgorszy możliwy przypadek, praktycznie nieosiągalny
    double dividingValueForMinimalGI = samplesWithOneFeatureAndResult.front().front(); // tymczasowo ustawiamy na cokolwiek - na wartość pierwszego elementu
    bool stumpReturnIfTrue = true; // w tym momencie będziemy sprawdzać, ile przypadków jest powyżej wartości granicznej, a ile poniżej, więc dobrze będzie, gdy nasze drzewo będzie zwracało po prostu true i false w zależności od podejmowanej decyzji
    bool stumpReturnIfFalse = false;
    for (size_t i = 0;
         i < samplesWithOneFeatureAndResult.size() - 1; ++i) { // - 1, gdyż liczymy średnią z dwóch sąsiednich, to jest pętla, w której badamy drzewa dla każdej wartości średniej i obliczamy dla nich współczynnik Giniego
        double dividingValue = (samplesWithOneFeatureAndResult.at(i).at(0) +
                                samplesWithOneFeatureAndResult.at(i + 1).at(0)) / 2.0;
        DecisionStump tempStump;
        tempStump.setAttributes(0, dividingValue, true, false); // 0, gdyż pod 0wym indeksem znajduje się analizowana cecha
        double giniImpurity = rateDecisionStump(samplesWithOneFeatureAndResult, tempStump);
        if (giniImpurity < minimalGiniImpurity) {
            minimalGiniImpurity = giniImpurity;
            dividingValueForMinimalGI = dividingValue;
            stumpReturnIfTrue = lastStumpReturnIfTrue_; // lastStumpReturnIfTrue_ jest liczone w metodzie rateDecisionStump
            stumpReturnIfFalse = lastStumpReturnIfFalse_;
        }
    }
    giniImputiryOfStump = minimalGiniImpurity;
    stump.setAttributes(indexOfDecisiveFeature, dividingValueForMinimalGI, stumpReturnIfTrue, stumpReturnIfFalse);
}

std::vector<bool> StumpCreator::tableOfCorrectClassification(const Samples& samples) {
    std::vector<bool> tableOfCorrectClassification(samples.size(), false); // na początku zakładamy, że wszystkieodpowiedzi są błędne
    for(size_t i = 0; i < samples.size(); ++i) {
        if(stump.predict(samples.at(i)) == featureToBool(samples.at(i).back()))
            tableOfCorrectClassification.at(i) = true; // gdy jakaś odpowiedź jest prawidłowa, to ustawiamy tę pozycję na true
    }
    return tableOfCorrectClassification;
}

double StumpCreator::rateDecisionStump(const Samples& samples, const DecisionStump& tempStump) {
    double stumpSaidTrueAndItIs = 0; // liczba przypadków, dla których drzewo powiedziało, że prawda i faktycznie była prawda
    double stumpSaidTrueButItIsNot = 0;
    double stumpSaidFalseAndItIs = 0;
    double stumpSaidFalseButItIsNot = 0;
    for(size_t i = 0; i < samples.size(); ++i) { // pętla zliczająca odpowiedzi drzewa na oreśloną cechę
        if(tempStump.predict(samples.at(i)) == true) {
            if(featureToBool(samples.at(i).back()) == true) stumpSaidTrueAndItIs += 1;
            else stumpSaidTrueButItIsNot += 1;
        }
        else {
            if(featureToBool(samples.at(i).back()) == false) stumpSaidFalseAndItIs += 1;
            else stumpSaidFalseButItIsNot += 1;
        }
    }
    if(stumpSaidTrueAndItIs > stumpSaidFalseButItIsNot) lastStumpReturnIfTrue_ = true; // tutaj decydujemy, jaką odpowiedź powinno dać drzewo, gdy wartość jest większa od decyzyjnej; ustalane jest to na podstawie tego, jakich odpowiedzi w tej gałęzi było więcej
    else lastStumpReturnIfTrue_ = false;
    if(stumpSaidFalseAndItIs > stumpSaidFalseButItIsNot) lastStumpReturnIfFalse_ = false; // analogicznie dla gałęzi mniejszej od wartości decyzyjnej
    else lastStumpReturnIfFalse_ = true;
    double stumpSaidTrue = stumpSaidTrueAndItIs + stumpSaidTrueButItIsNot; // liczba przypadków, gdy drzewo powiedziało, że prawda
    double stumpSaidFalse = stumpSaidFalseAndItIs + stumpSaidFalseButItIsNot; // liczba przypadków, gdy drzewo powiedziało, że fałs
    double giniImpurityForTrueLeaf;
    if(stumpSaidTrue <= 0) giniImpurityForTrueLeaf = 1; // unikamy dzielenia przez 0 linijkę poniżej
    else giniImpurityForTrueLeaf = 1 - pow(stumpSaidTrueAndItIs / stumpSaidTrue, 2) - pow(stumpSaidTrueButItIsNot / stumpSaidTrue, 2); // współczynik Giniego dla gałęzi większej od wartości decyzyjnej
    double giniImpurityForFalseLeaf;
    if(stumpSaidFalse <= 0) giniImpurityForFalseLeaf = 1; // unikamy dzielenia przez 0 linijkę poniżej
    else giniImpurityForFalseLeaf = 1 - pow(stumpSaidFalseAndItIs / stumpSaidFalse, 2) - pow(stumpSaidFalseButItIsNot / stumpSaidFalse, 2); // współczynik Giniego dla gałęzi mniejszej od wartości decyzyjnej
    double giniImpurity = giniImpurityForTrueLeaf * stumpSaidTrue / (stumpSaidTrue + stumpSaidFalse) +
                          giniImpurityForFalseLeaf * stumpSaidFalse / (stumpSaidTrue + stumpSaidFalse); // współczynnik dla całego drzewa (na podstawie średniej ważonej dla gałęzi)
    return giniImpurity;
}

bool operator < (const StumpCreator& s1, const StumpCreator& s2) {
    return s1.giniImputiryOfStump < s2.giniImputiryOfStump;
}