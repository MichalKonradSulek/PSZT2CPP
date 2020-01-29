//
// Created by michalsulek on 29.01.2020.
//

#include "AdaBoostAlgorithm.h"

StumpCreator AdaBoostAlgorithm::createStump(const Samples& samples, const std::vector<double>& weightsOfSamples) {
    if(samples.empty()) throw "no samples"; // jeśli nie ma przykładowych danych, to nie ma jak stworzyć drzewa
    if(samples.front().size() < 2) throw "no features"; // jeśli nie ma żadnych cech (jest tylko wynikowa), to nie ma jak stworzyć drzewa
    std::vector<StumpCreator> possibleStumps; // wektor zawierający drzewo dla każdej z cech
    for(size_t i = 0; i < samples.front().size() - 1; ++i) { // tworzenie drzewa dla każdej cechy (- 1, bo na ostatniej pozycji jest rezultat)
        possibleStumps.push_back(StumpCreator(samples, i)); // dodanie drzewa i jego oceny do wektora;
    }
    StumpCreator bestDecisionStump = *std::min_element(possibleStumps.begin(), possibleStumps.end());
    return bestDecisionStump;
}

double AdaBoostAlgorithm::calculateAmountOfSay(const std::vector<double>& weightsOfSamples, const std::vector<bool>& tableOfCorrectClassification) {
    double totalError = 0;
    for(size_t i = 0; i < weightsOfSamples.size(); ++i) {
        if(!tableOfCorrectClassification.at(i)) totalError += weightsOfSamples.at(i);
    }
    if(totalError <= 0) totalError = 0.0001; // jeśli zostawimy 0, to poniższe równanie się wyłoży
    else if(totalError >= 1) totalError = 0.9999; // jeśli zostawimy 1, to poniższe równanie się wyłoży
    double amountOfSay = 0.5 * log((1.0 - totalError) / totalError);
    return amountOfSay;
}

void AdaBoostAlgorithm::recalculateWeights(std::vector<double>& weightsOfSamples, const std::vector<bool>& tableOfCorrectClassification, double amountOfSay) {
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

void AdaBoostAlgorithm::normalizeWeights(std::vector<double>& weightsOfSamples) {
    double sum = 0;
    for(const auto& i: weightsOfSamples) {
        sum += i;
    }
    if(sum == 0) throw "weights = 0";
    for(auto& i: weightsOfSamples) {
        i = i / sum;
    }
}

void AdaBoostAlgorithm::changeSamples(Samples& samples, std::vector<double>& weightsOfSamples) {
    Samples newSamples(samples.size());
    std::vector<double> newWeights(samples.size());
    std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count()); // generator liczb losowych
    std::uniform_real_distribution<double> distribution(0, 1); // określenie zakresu generowanch liczb
    for(int i = 0; i < samples.size(); ++i) {
        double drawnNumber = distribution(generator);
        for(int j = 0; j < samples.size(); ++j) { //sprawdzanie do której "przegródki" wpadła wylosowana liczba
            if(drawnNumber <= weightsOfSamples.at(j)){
                newSamples.at(i) = samples.at(j);
                newWeights.at(i) = weightsOfSamples.at(j);
                break;
            }
            drawnNumber -= weightsOfSamples.at(j);
        }
    }
    normalizeWeights(newWeights);
    std::swap(samples, newSamples);
    std::swap(weightsOfSamples, newWeights);
}

void AdaBoostAlgorithm::trainAlgorithm(Samples samples, size_t numberOfStumps) {
    stumps_.clear();
    amountOfSay_.clear();
    std::vector<double> weightsOfSamples(samples.size(), 1.0 / samples.size()); // inicjalizujemy wagi równymi wartościami dla każdego z przypadków
    for(int i = 0; i < numberOfStumps; ++i) {
        StumpCreator stumpCreator = createStump(samples, weightsOfSamples);
        std::vector<bool> tableOfCorrectClassification = stumpCreator.tableOfCorrectClassification(samples);
        double amountOfSay = calculateAmountOfSay(weightsOfSamples, tableOfCorrectClassification);
        stumps_.push_back(stumpCreator.stump);
        amountOfSay_.push_back(amountOfSay);
        recalculateWeights(weightsOfSamples, tableOfCorrectClassification, amountOfSay);
        changeSamples(samples, weightsOfSamples);
    }
}

double AdaBoostAlgorithm::prediction(const RecordWithoutResult& record) {
    double amountOfSayForTrue = 0;
    double amountOfSayForFalse = 0;
    for(int i = 0; i < stumps_.size(); ++i) {
        if(stumps_.at(i).predict(record)) amountOfSayForTrue += amountOfSay_.at(i);
        else amountOfSayForFalse += amountOfSay_.at(i);
    }
    if(amountOfSayForTrue > amountOfSayForFalse) return boolToFeature(true);
    else return boolToFeature(false);
}
