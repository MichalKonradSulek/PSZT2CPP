//#define VERBOSE //należy zakomentować, by nie wyświetlać komunikatów "run0 run1... "

#include <iostream>

#include "tests.h"
#include "AdaBoostAlgorithm.h"

int main() {
    Samples samples = readSamples("../boston.txt", 14); //w przypadku nieznalezienia pliku poprawić ścieżkę
    const size_t INDEX_OF_DECISIVE_ATTRIBUTE = 14;
    const double DECISIVE_VALUE = 25;
    const double PART_OF_TEST_SAMPLES = 0.1;
    const size_t NUMBER_OF_DECISION_TREES = 5;

    printPrompt(INDEX_OF_DECISIVE_ATTRIBUTE, DECISIVE_VALUE, PART_OF_TEST_SAMPLES, NUMBER_OF_DECISION_TREES);
    auto result = runAlgorithmNTimes(samples, PART_OF_TEST_SAMPLES, NUMBER_OF_DECISION_TREES, DECISIVE_VALUE, 10);

    std::cout << "percent of correct guesses: " << result << std::endl;

    return 0;
}
