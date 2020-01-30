//
// Created by michalsulek on 29.01.2020.
//

#include "AdaBoostAlgorithm.h"

StumpCreator AdaBoostAlgorithm::createStump(const Samples& samples, const std::vector<double>& weightsOfSamples) const{
    if(samples.empty()) throw "no samples"; // jeśli nie ma przykładowych danych, to nie ma jak stworzyć drzewa
    if(samples.front().size() < 2) throw "no features"; // jeśli nie ma żadnych cech (jest tylko wynikowa), to nie ma jak stworzyć drzewa
    std::vector<StumpCreator> possibleStumps; // wektor zawierający drzewo dla każdej z cech
    for(size_t i = 0; i < samples.front().size() - 1; ++i) { // tworzenie drzewa dla każdej cechy (- 1, bo na ostatniej pozycji jest rezultat)
        possibleStumps.push_back(StumpCreator(samples, i, weightsOfSamples, dividingValueOfPredictedAttribute_)); // dodanie drzewa i jego oceny do wektora;
    }
    StumpCreator bestDecisionStump = *std::min_element(possibleStumps.begin(), possibleStumps.end());
    return bestDecisionStump;
}

double AdaBoostAlgorithm::calculateAmountOfSay(const std::vector<double>& weightsOfSamples, const std::vector<bool>& tableOfCorrectClassification) const{
    double totalError = 0;
    for(size_t i = 0; i < weightsOfSamples.size(); ++i) {
        if(!tableOfCorrectClassification.at(i)) totalError += weightsOfSamples.at(i);
    }
    if(totalError <= 0) totalError = 0.0001; // jeśli zostawimy 0, to poniższe równanie się wyłoży
    else if(totalError >= 1) totalError = 0.9999; // jeśli zostawimy 1, to poniższe równanie się wyłoży
    double amountOfSay = 0.5 * log((1.0 - totalError) / totalError);
    return amountOfSay;
}

void AdaBoostAlgorithm::recalculateWeights(std::vector<double>& weightsOfSamples, const std::vector<bool>& tableOfCorrectClassification, double amountOfSay) const{
    for(int i = 0; i < weightsOfSamples.size(); ++i) {
        if(tableOfCorrectClassification.at(i)) { // jeśli dany przypadek był wskazany dobrze, to zmniejszamy wagę
            weightsOfSamples.at(i) *= exp(-amountOfSay);
        }
        else { // jeśli źle, to zwiększamy
            weightsOfSamples.at(i) *= exp(amountOfSay);
        }
    }
    normalizeWeights(weightsOfSamples);
}

void AdaBoostAlgorithm::normalizeWeights(std::vector<double>& weightsOfSamples) const{
    double sum = 0;
    for(const auto& i: weightsOfSamples) {
        sum += i;
    }
    if(sum == 0) throw "weights = 0";
    for(auto& i: weightsOfSamples) {
        i = i / sum;
    }
}

void AdaBoostAlgorithm::trainAlgorithm(const Samples& samples, size_t numberOfStumps, double dividingValueOfPredictedAttribute) {
    dividingValueOfPredictedAttribute_ = dividingValueOfPredictedAttribute;
    stumps_.clear();
    amountOfSay_.clear();
    std::vector<double> weightsOfSamples(samples.size(), 1.0 / samples.size()); // inicjalizujemy wagi równymi wartościami dla każdego z przypadków
    for(int i = 0; i < numberOfStumps; ++i) {
//        std::cout << "iteration " << i << std::endl;
        StumpCreator stumpCreator = createStump(samples, weightsOfSamples);
        std::vector<bool> tableOfCorrectClassification = stumpCreator.tableOfCorrectClassification(samples);
//        for(int j = 0; j < samples.size(); ++j) {
//            showRecord(samples.at(j), weightsOfSamples.at(j));
//            std::cout << "toc: " << tableOfCorrectClassification.at(j) << std::endl;
//        }
        double amountOfSay = calculateAmountOfSay(weightsOfSamples, tableOfCorrectClassification);
        stumps_.push_back(stumpCreator.stump);
        amountOfSay_.push_back(amountOfSay);
        recalculateWeights(weightsOfSamples, tableOfCorrectClassification, amountOfSay);
//        changeSamples(samples, weightsOfSamples);
    }
}

double AdaBoostAlgorithm::prediction(const RecordWithoutResult& record) const{
    double amountOfSayForTrue = 0;
    double amountOfSayForFalse = 0;
    for(int i = 0; i < stumps_.size(); ++i) {
        if(stumps_.at(i).predict(record)) amountOfSayForTrue += amountOfSay_.at(i);
        else amountOfSayForFalse += amountOfSay_.at(i);
    }
    if(amountOfSayForTrue > amountOfSayForFalse) return boolToFeature(true, dividingValueOfPredictedAttribute_);
    else return boolToFeature(false, dividingValueOfPredictedAttribute_);
}
