//
// Created by michalsulek on 30.01.2020.
//

#ifndef PSZT2_TESTS_H
#define PSZT2_TESTS_H

#include <chrono>
#include <random>

#include "AdaBoostAlgorithm.h"

/** funkcja dzieląca próbki na grupę testową i trenującą w określonej proporcji
 * po wykonaniu funkcji zmienna samples zawiera tylko przypadki trenujace
 */
Samples takeTestSamplesFromSamples(Samples& samples, double partOfTestSamples) {
    size_t numberOfTestSamples = partOfTestSamples * samples.size();
    if(numberOfTestSamples == 0) return Samples();
    Samples testSamples;
    testSamples.reserve(numberOfTestSamples);
    std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count()); // generator liczb losowych
    for(size_t i = 0; i < numberOfTestSamples; ++i) {
        std::uniform_int_distribution<size_t> distribution(0, numberOfTestSamples - 1 - i); // określenie zakresu generowanch liczb
        size_t drawnNumber = distribution(generator);
        testSamples.push_back(samples.at(i));
        samples.erase(samples.begin() + drawnNumber);
    }
    return testSamples;
}

/** funkcja sprawdza, czy predykcja jest zgodna z faktycznym wynikiem
 * w tym celu obie wartości sprowadzone są do zmiennej boolowskiej, decydującej, czy dana wartość
 * jest większa lub nie od wartości granicznej
 */
bool isPredictionCorrect(double predictedValue, double realValue, double decisiveValue) {
    return featureToBool(predictedValue, decisiveValue) == featureToBool(realValue, decisiveValue);
}

/** funkcja uruchamiająca algorytm n razy dla danych ustawień algorytmu
 * zwracany jest wektor liczb typu double określających procent poprawnych wyników w każdej z prób
 */
std::vector<double> runAlgorithmNTimes(const Samples& samples, double partOfTestSamples,
        size_t numberOfDecisionTrees, double dividingValueOfPredictedAttribute, unsigned numberOfRuns) {
    std::vector<double> results;
    for(unsigned i = 0; i < numberOfRuns; ++i) {
#ifdef VERBOSE
        std::cout << "run" << i << " " << std::flush;
#endif
        Samples trainingSamples, testSamples;
        trainingSamples = samples;
        testSamples = takeTestSamplesFromSamples(trainingSamples, partOfTestSamples);
        AdaBoostAlgorithm adaBoost;
        adaBoost.trainAlgorithm(trainingSamples, numberOfDecisionTrees, dividingValueOfPredictedAttribute);
        double numberOfMistakes = 0;
        for(const auto & testSample : testSamples) {
            if(!isPredictionCorrect(adaBoost.prediction(testSample), testSample.back(), dividingValueOfPredictedAttribute)) ++numberOfMistakes;
        }
        double percentOfCorrectPredictions = (1.0 - (numberOfMistakes / testSamples.size())) * 100;
        results.push_back(percentOfCorrectPredictions);
    }
#ifdef VERBOSE
    std::cout << std::endl;
#endif
    return results;
}

std::ostream& operator <<(std::ostream& os, const std::vector<double>& vec) {
    for(const auto& i: vec) {
        os << i << " ";
    }
    return os;
}

void printPrompt(size_t decisiveAttribute, double dividingValue, double partOfTestSamples, size_t nOfDecisionTrees) {
    std::cout << "decisiveAttribute:" << decisiveAttribute << " dividingValue:" << dividingValue << " partOfTestSamples:" << partOfTestSamples << " nOfDecisionTrees:" << nOfDecisionTrees << std::endl;
}

#endif //PSZT2_TESTS_H
