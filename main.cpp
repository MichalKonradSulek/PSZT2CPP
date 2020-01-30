#define VERBOSE //należy zakomentować, by nie wyświetlać komunikatów "run0 run1... "

#include <iostream>

#include "tests.h"
#include "AdaBoostAlgorithm.h"


int main() {
    Samples samples = readSamples("../boston.txt", 14); //w przypadku nieznalezienia pliku poprawić ścieżkę

    //prediction of the last attribute

    size_t indexOfDecisiveAttribute = 13;
    double dividingValueOfPredictedAttribute = 25;
    double partOfTestSamples = 0.1;
    size_t numberOfDecisionTrees = 5;

    printPrompt(indexOfDecisiveAttribute, dividingValueOfPredictedAttribute, partOfTestSamples, numberOfDecisionTrees);
    auto result = runAlgorithmNTimes(samples, partOfTestSamples, numberOfDecisionTrees, dividingValueOfPredictedAttribute, 10, indexOfDecisiveAttribute);
    std::cout << "percent of correct guesses: ";
    std::cout << result << std::endl;

    //prediction of the third attribute

    indexOfDecisiveAttribute = 2;
    dividingValueOfPredictedAttribute = 15;
    partOfTestSamples = 0.5;

    printPrompt(indexOfDecisiveAttribute, dividingValueOfPredictedAttribute, partOfTestSamples, numberOfDecisionTrees);
    result = runAlgorithmNTimes(samples, partOfTestSamples, numberOfDecisionTrees, dividingValueOfPredictedAttribute, 10, indexOfDecisiveAttribute);
    std::cout << "percent of correct guesses: ";
    std::cout << result << std::endl;

    return 0;
}
