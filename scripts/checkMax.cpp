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

const int seed = 555;
std::default_random_engine gen{seed};
// std::default_random_engine gen{std::random_device()};

extern "C" double _tanh(double) __attribute__((weak));
extern "C" double _gelu(double) __attribute__((weak));
extern "C" double _swish(double) __attribute__((weak));
extern "C" double _mish(double) __attribute__((weak));
extern "C" double _sigmoid(double) __attribute__((weak));
extern "C" double _softplus(double) __attribute__((weak));
extern "C" double _expm1(double) __attribute__((weak));
extern "C" double _exp(double) __attribute__((weak));
extern "C" double _erf(double) __attribute__((weak));

typedef double (*unary_fp)(double);
struct Entry { const char *name; double (*func)(double); };

static struct Entry table[] = {
    {"_gelu", _gelu},
    {"_tanh", _tanh},
    {"_swish", _swish},
    {"_mish", _mish},
    {"_sigmoid", _sigmoid},
    {"_softplus", _softplus},
    {"_expm1", _expm1},
    {"_exp", _exp},
    {"_erf", _erf},
    {NULL, NULL}
};

double randNum(double _min, double _max) {
    std::uniform_int_distribution<> dist_sign(0, 1);  // 0 or 1
    std::uniform_real_distribution<double> dist(_min, _max);
    double val = dist(gen);
    return val * (dist_sign(gen) == 0 ? 1.0 : -1.0);
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <funcname> <min> <max>\n", argv[0]);
        return 1;
    }

    int cnt = 10000;

    const char *fname = argv[1];
    unary_fp f = nullptr;
    for (Entry *e = table; e->name; ++e)
        if (e->func && strcmp(fname, e->name) == 0) { f = e->func; break; }

    if (!f) {
        std::cout << "Unknown function: " << fname << "\n";
        return 1;
    }

    double _min=std::stoi(argv[2]), _max=std::stoi(argv[3]);
    std::vector<double> res(cnt, 0.0);
    double abs_max = 0.0;

    for (int i=0; i<cnt; i++) {
        double x = randNum(_min, _max);
        res[i] = f(x); 

        // std::cout << fname << "(" << x << ") = " << res[i] << " (expected = " << x/sqrt(2) << ")\n";
        
        abs_max = std::abs(res[i]) > abs_max ? std::abs(res[i]) : abs_max;

    }

    std::cout << abs_max << "\n";

    return 0;
}