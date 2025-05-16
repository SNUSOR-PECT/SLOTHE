#include <sstream>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstdio>
#include <string>
#include <cstring>
#include <vector>
#include <cmath>
#include <cstdint>
#include <random>
#include <iomanip>
#include <utility>
#include <map>

extern "C" double _tanh(double); // target function

void loadVals(std::vector<double>& vec, std::string path) {
    std::ifstream ifs(path);
    if (!ifs) {
        std::cerr << "Failed to open file.\n";
        // return 0;
    } else {
        double val;
        while (ifs >> val) {
            vec.push_back(val);
        }
    }
}

int main(int argc, char* argv[]) {
    
    int ex = std::stoi(argv[1]);
    double prec = std::pow(10, -ex);

    // T0 T1 for delta_1, F0 F1 for delta_2
    std::vector<double> T0, T1, F0, F1;
    std::string pathT0 = "../T0.txt";
    std::string pathT1 = "../T1.txt";
    std::string pathF0 = "../F0.txt";
    std::string pathF1 = "../F1.txt";
    loadVals(T0, pathT0);
    loadVals(T1, pathT1);
    loadVals(F0, pathF0);
    loadVals(F1, pathF1);

    double delta_1 = 0.0, delta_2 = 0.0;
    for (int i=0; i<T0.size(); i++) {
        double diff1 = std::abs(T0[i] - T1[i]);
        delta_1 = diff1 > delta_1 ? diff1 : delta_1;
        double diff2 = std::abs(F0[i] - F1[i]);
        delta_2 = diff2 > delta_2 ? diff2 : delta_2;
    }
    double delta = delta_1 < delta_2 ? delta_1 : delta_2;
    bool isMergeable = (delta < pow(10, -std::stoi(argv[1])));

    if (isMergeable) {
        if (delta_1 <= delta_2) std::cout << "1";
        else std::cout << "0";
    } else {
        std::cout << "-1";
    }

    return isMergeable; // mergeable

}