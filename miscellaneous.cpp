//
// Created by michalsulek on 29.01.2020.
//

#include <fstream>

#include "miscellaneous.h"

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
    if(feature < 1) return false;
    else return true;
}
double boolToFeature(bool b) {
    if(b) return 1;
    else return 0;
}

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