#include <iostream>


#include "AdaBoostAlgorithm.h"

int main() {
    Samples samples = readSamples("../boston.txt", 7);
    moveBackColumn(samples, 3);
    AdaBoostAlgorithm adaBoost;
    adaBoost.trainAlgorithm(samples, 5);

//    double nOf0 = 0;
//    for(size_t i = 0; i < samples.size(); ++i) {
//        if(samples.at(i).back() == 0) ++nOf0;
//    }

    double numberOfMistakes = 0;
    for(size_t i = 0; i < samples.size(); ++i) {
        std::cout << "index: " << i << "\t";
        std::cout << "adaBoost: " << adaBoost.prediction(samples.at(i)) << "\t";
        std::cout << "reality : " << samples.at(i).back() << std::endl;
        if(adaBoost.prediction(samples.at(i)) != samples.at(i).back() == 0) ++numberOfMistakes;
    }
    std::cout << "percent of mistakes: " << numberOfMistakes * 100 / samples.size() << std::endl;
//    std::cout << "nOf0: " << nOf0 << std::endl;
    return 0;
}
