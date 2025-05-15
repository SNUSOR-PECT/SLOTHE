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

#define debug 0

extern "C" double _tanh(double); // target function

const int seed = 555;
std::default_random_engine gen{seed};
// std::default_random_engine gen{std::random_device()};

double randNum(double _min, double _max) {
    std::uniform_int_distribution<> dist_sign(0, 1);  // 0 or 1
    std::uniform_real_distribution<double> dist(_min, _max);
    double val = dist(gen);
    return val * (dist_sign(gen) == 0 ? 1.0 : -1.0);
}

int main(int argc, char* argv[]) {
    int cnt = 100;
    double _min=-22, _max=22;
    std::vector<double> res(cnt, 0.0);

    for (int i=0; i<cnt; i++) {
        double x = randNum(_min, _max);
        double y = tanh(x);
        res[i] = _tanh(x);
        std::cout << y << ", " << res[i] << "\n";
        // double diff = std::abs(y - res[i]);
        // std::cout << diff << "\n";
    }
    

    return 0;
}