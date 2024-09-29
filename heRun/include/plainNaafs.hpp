#pragma once

#include <functional>
#include <iostream>
#include <algorithm>
#include <vector>
#include <cmath>

// Non-Arithmetic Activation Functions
double ReLU(double x);      // return max(x, 0)
double Swish(double x);     // return x * sigmoid(x)
double Tanh(double x);      // return tanh(x)
double Sigmoid(double x);   // return 1 / {exp(-x)+1}
double GeLU(double x);      // return x/2 * (1 + erf(x/sqrt(2)))
double Softplus(double x);  // return  log(1 + exp(x))

// Inner Functions
double Exp(double x);       // return exp(x)
double Inv(double x);       // return 1/x
double Abs(double x);       // return |x|
double Erf(double x);       // return erf(x)
double Log(double x);       // return log(x)

// Comparison
double Compare(double x, double y);     // return x > y ? 1.0 : 0.0

// Removed+Optimized Functions
double _Tanh(double x);
double _Inv(double x, int d);