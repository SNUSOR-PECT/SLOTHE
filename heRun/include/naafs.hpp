#pragma once

#include <functional>
#include <iostream>
#include <vector>
#include <cmath>

#include "NTL/RR.h"
#include "seal/seal.h"

#include "helper.hpp"

using namespace seal;
using namespace NTL;

// for aSOR
#define maxIter 100

/*
 * PA : Polynomial Approximation
 * NA : Numerical Approximation (not only by PA)
 * LA : Lazy Approximation (Ours)
*/

// Non-Arithmetic Activation Functions
// void ReLU(double x);      // return max(x, 0)
// void Swish(double x);     // return x * sigmoid(x)
void Tanh_PA(std::vector<double>& K, long n, SEALContext &context, Encryptor& encryptor, Evaluator& evaluator, Decryptor& decryptor, CKKSEncoder& encoder, PublicKey& pk, SecretKey& sk, RelinKeys& rlks, Ciphertext& ctxt_in, Ciphertext& ctxt_out, Timer& timer);      // return tanh(x)
void Tanh_LA(std::vector<double>& K, long n, SEALContext &context, Encryptor& encryptor, Evaluator& evaluator, Decryptor& decryptor, CKKSEncoder& encoder, PublicKey& pk, SecretKey& sk, RelinKeys& rlks, Ciphertext& ctxt_in, Ciphertext& ctxt_out, Timer& timer);      // return tanh(x)
void Sigmoid_PA(std::vector<double>& K, long n, SEALContext &context, Encryptor& encryptor, Evaluator& evaluator, Decryptor& decryptor, CKKSEncoder& encoder, PublicKey& pk, SecretKey& sk, RelinKeys& rlks, Ciphertext& ctxt_in, Ciphertext& ctxt_out, Timer& timer);
void GeLU_PA(std::vector<double>& K, long n, SEALContext &context, Encryptor& encryptor, Evaluator& evaluator, Decryptor& decryptor, CKKSEncoder& encoder, PublicKey& pk, SecretKey& sk, RelinKeys& rlks, Ciphertext& ctxt_in, Ciphertext& ctxt_out, Timer& timer);
void Softplus_PA(std::vector<double>& K, long n, SEALContext &context, Encryptor& encryptor, Evaluator& evaluator, Decryptor& decryptor, CKKSEncoder& encoder, PublicKey& pk, SecretKey& sk, RelinKeys& rlks, Ciphertext& ctxt_in, Ciphertext& ctxt_out, Timer& timer);
// void Softplus(double x);  // return  log(1 + exp(x))

// Inner Functions
void Exp_PA(std::vector<double>& K, long n, SEALContext &context, Encryptor& encryptor, Evaluator& evaluator, Decryptor& decryptor, CKKSEncoder& encoder, PublicKey& pk, SecretKey& sk, RelinKeys& rlks, Ciphertext& ctxt_in, Ciphertext& ctxt_out, Timer& timer);       // return exp(x)
void Expm1_NA(std::vector<double>& K, long n, SEALContext &context, Encryptor& encryptor, Evaluator& evaluator, Decryptor& decryptor, CKKSEncoder& encoder, PublicKey& pk, SecretKey& sk, RelinKeys& rlks, Ciphertext& ctxt_in, Ciphertext& ctxt_out, Timer& timer);   // return exp(x)-1
void Inv_NA(std::vector<double>& K, long n, SEALContext &context, Encryptor& encryptor, Evaluator& evaluator, Decryptor& decryptor, CKKSEncoder& encoder, PublicKey& pk, SecretKey& sk, RelinKeys& rlks, Ciphertext& ctxt_in, Ciphertext& ctxt_out, Timer& timer);       // return 1/x
// void Abs(double x);       // return |x|
// void Erf(double x);       // return erf(x)
// void Log(double x);       // return log(x)

// Comparison
// void Compare(double x, double y);     // return x > y ? 1.0 : 0.0

// Helper functions
void getRelaxationFactor(std::function<double(double)> f, double alpha, double epsilon, std::vector<double>& K);
void compute_all_powers(const Ciphertext& ctxt, int deg, Evaluator &evaluator, RelinKeys &rlks, std::vector<Ciphertext>& powers);
void evalPoly(std::vector<double> coeffs, Encryptor &encryptor, Evaluator &evaluator, Decryptor &decryptor, CKKSEncoder &encoder, PublicKey &pk, SecretKey &sk, RelinKeys &rlks, Ciphertext& ctxt_in, Ciphertext& ctxt_out, Timer& timer);
void evalPolyOdd(std::vector<double> coeffs, Encryptor &encryptor, Evaluator &evaluator, Decryptor &decryptor, CKKSEncoder &encoder, PublicKey &pk, SecretKey &sk, RelinKeys &rlks, Ciphertext& ctxt_in, Ciphertext& ctxt_out, Timer& timer);
