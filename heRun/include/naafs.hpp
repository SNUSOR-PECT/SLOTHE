#pragma once

#include <functional>
#include <iostream>
#include <vector>
#include <cmath>

#include "NTL/RR.h"
#include "seal/seal.h"

using namespace seal;
using namespace NTL;

// for aSOR
#define maxIter 100

// Non-Arithmetic Activation Functions
// void ReLU(double x);      // return max(x, 0)
// void Swish(double x);     // return x * sigmoid(x)
void Tanh(std::vector<double>& K, long n, SEALContext &context, Encryptor& encryptor, Evaluator& evaluator, Decryptor& decryptor, CKKSEncoder& encoder, PublicKey& pk, SecretKey& sk, RelinKeys& rlks, Ciphertext& ctxt_in, Ciphertext& ctxt_out);      // return tanh(x)
// void Sigmoid(double x);   // return 1 / {exp(-x)+1}
// void GeLU(double x);      // return x/2 * (1 + erf(x/sqrt(2)))
// void Softplus(double x);  // return  log(1 + exp(x))

// Inner Functions
// void Exp(double x);       // return exp(x)
void Inv(std::vector<double>& K, long n, SEALContext &context, Encryptor& encryptor, Evaluator& evaluator, Decryptor& decryptor, CKKSEncoder& encoder, PublicKey& pk, SecretKey& sk, RelinKeys& rlks, Ciphertext& ctxt_in, Ciphertext& ctxt_out);       // return 1/x
// void Abs(double x);       // return |x|
// void Erf(double x);       // return erf(x)
// void Log(double x);       // return log(x)

// Comparison
// void Compare(double x, double y);     // return x > y ? 1.0 : 0.0

// Helper functions
void getRelaxationFactor(std::function<double(double)> f, double alpha, double epsilon, std::vector<double>& K);