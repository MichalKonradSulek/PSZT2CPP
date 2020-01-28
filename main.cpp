#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <random>
#include <chrono>
#include <fstream>

typedef std::vector<double> RecordWithResult; ///<kontener na jeden obiekt cech z wynikiem; szukana cecha jest na końcu
typedef std::vector<double> RecordWithoutResult; ///<kontener na jeden obiekt cech bez wyniku
typedef std::vector<RecordWithResult> Samples; ///<kontener z próbkami

bool operator < (const RecordWithResult& r1, const RecordWithResult& r2) { ///<operator wymagany przez funkcję std::sort
    if(r1.size() == r2.size()) {
        for(size_t i = 0; i < r1.size(); ++i) {
            if(r1.at(i) < r2.at(i)) return true;
            else if(r1.at(i) > r2.at(i)) return false;
        }
        return false;
    }
    else return r1.size() < r2.size();
}

bool featureToBool(double feature) {
    if(feature < 25) return false;
    else return true;
}
double boolToFeature(bool b) {
    if(b) return 1;
    else return 0;
}

class DecisionStump {
public:
    DecisionStump() = default;
    void setAttributes(size_t indexOfDecisiveFeature, double dividingValue, bool resultIfBiggerValue, bool resultIfSmallerValue) {
        indexOfDecisiveFeature_ = indexOfDecisiveFeature;
        dividingValue_ = dividingValue;
        resultIfBiggerValue_ = resultIfBiggerValue;
        resultIfSmallerValue_ = resultIfSmallerValue;
    }
    bool predict(const RecordWithoutResult& sample) const { // also works with RecordWithResult
        if(sample.at(indexOfDecisiveFeature_) > dividingValue_) return resultIfBiggerValue_;
        else return resultIfSmallerValue_;
    }
private:
    size_t indexOfDecisiveFeature_;
    double dividingValue_;
    bool resultIfBiggerValue_;
    bool resultIfSmallerValue_;
};

struct StumpCreator {
    StumpCreator(const Samples& samples, size_t indexOfDecisiveFeature) {
        if(samples.empty()) throw "no samples"; // jeśli nie ma próbek, to nie stworzy się drzewa
        if(indexOfDecisiveFeature >= samples.front().size() - 1) throw "no features"; // jeśli nie ma cech, to nie stworzy się drzewa
        Samples samplesWithOneFeatureAndResult(samples.size(), RecordWithResult(2)); // 2, bo każdy rekord będzie miał 2 kolumny
        for(size_t i = 0; i < samples.size(); ++i) {
            samplesWithOneFeatureAndResult.at(i).at(0) = samples.at(i).at(indexOfDecisiveFeature); // jedna z cech zadanych
            samplesWithOneFeatureAndResult.at(i).at(1) = samples.at(i).back(); // cecha szukana
        }
        std::sort(samplesWithOneFeatureAndResult.begin(), samplesWithOneFeatureAndResult.end());

        double minimalGiniImpurity = 1;
        double dividingValueForMinimalGI = samplesWithOneFeatureAndResult.front().front();
        bool stumpReturnIfTrue = true;
        bool stumpReturnIfFalse = false;
        for(size_t i = 0; i < samplesWithOneFeatureAndResult.size() - 1; ++i) { // - 1, gdyż liczymy średnią z dwóch sąsiednich
            double dividingValue = (samplesWithOneFeatureAndResult.at(i).at(0) +
                    samplesWithOneFeatureAndResult.at(i + 1).at(0)) / 2.0;
            DecisionStump tempStump;
            tempStump.setAttributes(0, dividingValue, true, false);
            double giniImpurity = rateDecisionStump(samplesWithOneFeatureAndResult, tempStump);
            if(giniImpurity < minimalGiniImpurity) {
                minimalGiniImpurity = giniImpurity;
                dividingValueForMinimalGI = dividingValue;
                stumpReturnIfTrue = lastStumpReturnIfTrue_;
                stumpReturnIfFalse = lastStumpReturnIfFalse_;
            }
        }
        giniImputiryOfStump = minimalGiniImpurity;
        stump.setAttributes(indexOfDecisiveFeature, dividingValueForMinimalGI, stumpReturnIfTrue, stumpReturnIfFalse);
    } //TODO constructor

    std::vector<bool> tableOfCorrectClassification(const Samples& samples) {
        std::vector<bool> tableOfCorrectClassification(samples.size(), false);
        for(size_t i = 0; i < samples.size(); ++i) {
            if(stump.predict(samples.at(i)) == featureToBool(samples.at(i).back()))
                tableOfCorrectClassification.at(i) = true;
        }
        return tableOfCorrectClassification;
    }

    DecisionStump stump;
    double giniImputiryOfStump;

private:
    double rateDecisionStump(const Samples& samples, const DecisionStump& tempStump) {
        double stumpSaidTrueAndItIs = 0; // liczba przypadków, dla których drzewo powiedziało, że prawda i faktycznie była prawda
        double stumpSaidTrueButItIsNot = 0;
        double stumpSaidFalseAndItIs = 0;
        double stumpSaidFalseButItIsNot = 0;
        for(size_t i = 0; i < samples.size(); ++i) {
            if(tempStump.predict(samples.at(i)) == true) {
                if(featureToBool(samples.at(i).back()) == true) stumpSaidTrueAndItIs += 1;
                else stumpSaidTrueButItIsNot += 1;
            }
            else {
                if(featureToBool(samples.at(i).back()) == false) stumpSaidFalseAndItIs += 1;
                else stumpSaidFalseButItIsNot += 1;
            }
        }
        if(stumpSaidTrueAndItIs > stumpSaidFalseButItIsNot) lastStumpReturnIfTrue_ = true;
        else lastStumpReturnIfTrue_ = false;
        if(stumpSaidFalseAndItIs > stumpSaidFalseButItIsNot) lastStumpReturnIfFalse_ = false;
        else lastStumpReturnIfFalse_ = true;

        double stumpSaidTrue = stumpSaidTrueAndItIs + stumpSaidTrueButItIsNot; // liczba przypadków, gdy drzewo powiedziało, że prawda
        double stumpSaidFalse = stumpSaidFalseAndItIs + stumpSaidFalseButItIsNot; // liczba przypadków, gdy drzewo powiedziało, że fałs
        double giniImpurityForTrueLeaf;
        if(stumpSaidTrue <= 0) giniImpurityForTrueLeaf = 1; // unikamy dzielenia przez 0 linijkę poniżej
        else giniImpurityForTrueLeaf = 1 - pow(stumpSaidTrueAndItIs / stumpSaidTrue, 2) - pow(stumpSaidTrueButItIsNot / stumpSaidTrue, 2);
        double giniImpurityForFalseLeaf;
        if(stumpSaidFalse <= 0) giniImpurityForFalseLeaf = 1; // unikamy dzielenia przez 0 linijkę poniżej
        else giniImpurityForFalseLeaf = 1 - pow(stumpSaidFalseAndItIs / stumpSaidFalse, 2) - pow(stumpSaidFalseButItIsNot / stumpSaidFalse, 2);
        double giniImpurity = giniImpurityForTrueLeaf * stumpSaidTrue / (stumpSaidTrue + stumpSaidFalse) +
                              giniImpurityForFalseLeaf * stumpSaidFalse / (stumpSaidTrue + stumpSaidFalse);
        return giniImpurity;
    }

    bool lastStumpReturnIfTrue_;
    bool lastStumpReturnIfFalse_;
};

bool operator < (const StumpCreator& s1, const StumpCreator& s2) { // operator wymagany przez funkcję min_element
    return s1.giniImputiryOfStump < s2.giniImputiryOfStump;
}

class AdaBoostAlgorithm {
private:
    StumpCreator createStump(const Samples& samples, const std::vector<double>& weightsOfSamples) {
        if(samples.empty()) throw "no samples"; // jeśli nie ma przykładowych danych, to nie ma jak stworzyć drzewa
        if(samples.front().size() < 2) throw "no features"; // jeśli nie ma żadnych cech (jest tylko wynikowa), to nie ma jak stworzyć drzewa
        std::vector<StumpCreator> possibleStumps; // wektor zawierający drzewo dla każdej z cech
        for(size_t i = 0; i < samples.front().size() - 1; ++i) { // tworzenie drzewa dla każdej cechy (- 1, bo na ostatniej pozycji jest rezultat)
            possibleStumps.push_back(StumpCreator(samples, i)); // dodanie drzewa i jego oceny do wektora;
        }
        StumpCreator bestDecisionStump = *std::min_element(possibleStumps.begin(), possibleStumps.end()); //TODO czy napewno min?
        return bestDecisionStump;
    }

    double rateDecisionStump(const Samples& samples, const StumpCreator& stump);


    double calculateAmountOfSay(const std::vector<double>& weightsOfSamples, const std::vector<bool>& tableOfCorrectClassification) {
        double totalError = 0;
        for(size_t i = 0; i < weightsOfSamples.size(); ++i) {
            if(!tableOfCorrectClassification.at(i)) totalError += weightsOfSamples.at(i);
        }
        if(totalError <= 0) totalError = 0.0001; // jeśli zostawimy 0, to poniższe równanie się wyłoży
        else if(totalError >= 1) totalError = 0.9999; // jeśli zostawimy 1, to poniższe równanie się wyłoży
        double amountOfSay = 0.5 * log((1.0 - totalError) / totalError);
        return amountOfSay;
    }

    void recalculateWeights(std::vector<double>& weightsOfSamples, const std::vector<bool>& tableOfCorrectClassification, double amountOfSay) {
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

    void normalizeWeights(std::vector<double>& weightsOfSamples) {
        double sum = 0;
        for(const auto& i: weightsOfSamples) {
            sum += i;
        }
        if(sum == 0) throw "weights = 0";
        for(auto& i: weightsOfSamples) {
            i = i / sum;
        }
    }

    void changeSamples(Samples& samples, std::vector<double>& weightsOfSamples) {
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

public:
    void trainAlgorithm(Samples samples, size_t numberOfStumps) {
        stumps_.clear();
        amountOfSay_.clear();
        std::vector<double> weightsOfSamples(samples.size(), 1.0 / samples.size());
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

    double prediction(const RecordWithoutResult& record) {
        double amountOfSayForTrue = 0;
        double amountOfSayForFalse = 0;
        for(int i = 0; i < stumps_.size(); ++i) {
            if(stumps_.at(i).predict(record)) amountOfSayForTrue += amountOfSay_.at(i);
            else amountOfSayForFalse += amountOfSay_.at(i);
        }
        if(amountOfSayForTrue > amountOfSayForFalse) return boolToFeature(true);
        else return boolToFeature(false);
    }


private:
    std::vector<DecisionStump> stumps_;
    std::vector<double> amountOfSay_;
};

Samples readSamples(const std::string& file, size_t nOfColumns) {
    std::ifstream fin(file);
    if(!fin.good()) throw "bad file";
    Samples samples;
    samples.reserve(600);
    double number;
    int i = 0;
    while(fin >> number) {
        if(i % nOfColumns == 0) samples.push_back(RecordWithResult());
        samples.back().push_back(number);
        ++i;
    }
    return samples;
}

void showRecord(const RecordWithResult& record) {
    for(const auto& i: record) {
        std::cout << i << "\t";
    }
}

void showSamples(const Samples& samples) {
    for(const auto& record: samples) {
        showRecord(record);
        std::cout << std::endl;
    }
}

void moveBackColumn(Samples& samples, size_t columnIndex) {
    for(auto& record: samples) {
        record.push_back(record.at(columnIndex));
        record.erase(record.begin() + columnIndex);
    }
}

int main() {
    Samples samples = readSamples("../boston.txt", 14);
//    moveBackColumn(samples, 3);
    AdaBoostAlgorithm adaBoost;
    adaBoost.trainAlgorithm(samples, 10);

//    double nOf0 = 0;
//    for(size_t i = 0; i < samples.size(); ++i) {
//        if(samples.at(i).back() == 0) ++nOf0;
//    }

    double numberOfMistakes = 0;
    for(size_t i = 0; i < samples.size(); ++i) {
        std::cout << "index: " << i << "\t";
        std::cout << "adaBoost: " << adaBoost.prediction(samples.at(i)) << "\t";
        std::cout << "reality : " << samples.at(i).back() << std::endl;
        if(adaBoost.prediction(samples.at(i)) == 1 && samples.at(i).back() < 25) ++numberOfMistakes;
        else if(adaBoost.prediction(samples.at(i)) == 0 && samples.at(i).back() >= 25) ++numberOfMistakes;
    }
    std::cout << "percent of mistakes: " << numberOfMistakes * 100 / samples.size() << std::endl;
//    std::cout << "nOf0: " << nOf0 << std::endl;
    return 0;
}
