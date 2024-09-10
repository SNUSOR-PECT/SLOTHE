#pragma once

#include <functional>
#include <iostream>
#include <vector>
#include <cmath>

#include "NTL/RR.h"
#include "seal/seal.h"

using namespace seal;
using namespace NTL;

long ShowFailure_Inverse(Decryptor &decryptor, CKKSEncoder &encoder, Ciphertext& cipher, std::vector<double>& x, long alpha, long n);