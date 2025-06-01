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
#include <unordered_map>
#include <map>

#include "../math/naf.h"

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
extern "C" double f(double) __attribute__((weak));

typedef double (*unary_fp)(double);
struct Entry { const char *name; double (*func)(double); };

static struct Entry table[] = {
    {"_gelu", _gelu},
    {"_tanh", _tanh},
    {"_swish", _swish},
    {"_mish", _mish},
    {"_sigmoid", _sigmoid},
    {"_softplus", _softplus},
    {"_expm1", expm1},
    {"_exp", exp},
    {"_erf", erf},
    {"f", f},
    {NULL, NULL}
};

using Func = double(*)(double);
static const std::unordered_map<std::string, Func> tblOrigin = {
    {"_gelu", Gelu },
    {"_tanh", Tanh },
    {"_swish", Swish},
    {"_mish", Mish},
    {"_sigmoid", Sigmoid},
    {"_softplus", Softplus},
    {"_expm1", Expm1},
    {"_exp", Exp},
    {"_erf", Erf},
};

double callOrigin(const std::string& s, double x) {
    auto it = tblOrigin.find(s);
    if (it == tblOrigin.end())
        throw std::runtime_error("[Origin] Unknown function: " + s);
    return it->second(x);
}

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
        // PA    
        const char *_fname = "f";
        for (Entry *e = table; e->name; ++e)
            if (e->func && strcmp(_fname, e->name) == 0) { f = e->func; break; }
    }

    double _min=std::stoi(argv[2]), _max=std::stoi(argv[3]);
    std::vector<double> res(cnt, 0.0);

    double errMax = -100.0;
    for (int i=0; i<cnt; i++) {
        double x = randNum(_min, _max);
        double y = callOrigin(std::string(fname), x);
        res[i] = f(x);

        double diff = std::abs(y - res[i]);
        errMax = errMax > diff ? errMax : diff;
    }
    
    std::cout << errMax << "\n";

    return 0;
}