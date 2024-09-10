#pragma once

#include <functional>
#include <iostream>
#include <chrono>
#include <vector>
#include <stack>
#include <cmath>

#include "NTL/RR.h"
#include "seal/seal.h"

using namespace std::chrono;
using namespace seal;
using namespace NTL;

class Timer {
    public:
        void start(void);
        void end(void);

    public:
        std::stack<high_resolution_clock::time_point> t_starts;
        high_resolution_clock::time_point t_start, t_end;
    private:
        milliseconds t_diff;
};

long ShowFailure_Inverse(Decryptor &decryptor, CKKSEncoder &encoder, Ciphertext& cipher, std::vector<double>& x, long alpha, long n);