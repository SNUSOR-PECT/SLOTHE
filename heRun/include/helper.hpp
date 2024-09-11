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

void checkPrecision(long n, long log_modulus, SEALContext& context, Encryptor& encryptor, Evaluator& evaluator, Decryptor& decryptor, CKKSEncoder& encoder, PublicKey& pk, SecretKey& sk, RelinKeys& rlks);

// Check failure under given alpha (precision)
long ShowFailure_Tanh(Decryptor &decryptor, CKKSEncoder &encoder, Ciphertext& cipher, std::vector<double>& x, long alpha, long n);
long ShowFailure_Exp(Decryptor &decryptor, CKKSEncoder &encoder, Ciphertext& ctxt, std::vector<double>& x, long alpha, long n);
long ShowFailure_Expm1(Decryptor &decryptor, CKKSEncoder &encoder, Ciphertext& cipher, std::vector<double>& x, long alpha, long n);
long ShowFailure_Inverse(Decryptor &decryptor, CKKSEncoder &encoder, Ciphertext& cipher, std::vector<double>& x, long alpha, long n);

// Print functions
void print_part(const std::vector<double>& m_x, long sparse_slots, size_t start, size_t end);
void decrypt_and_print_part(const Ciphertext& ctxt_in, Decryptor& decryptor, CKKSEncoder& encoder, long sparse_slots, size_t start, size_t end);