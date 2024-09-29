#include "plainNaafs.hpp"

////////////////////////////////////////////////
//////        Activation Functions        //////
////////////////////////////////////////////////

double ReLU(double x) {
    return x > 0.0 ? x : 0.0; // std::max(x, 0);
}
double Swish(double x) {
    return x * Sigmoid(x);
}
double Tanh(double x) {
    return std::tanh(x);
}
double Sigmoid(double x) {
    return 1 / (Exp(-x) + 1);
}
double GeLU(double x) {
    return x/2 * (1 + Erf(x / std::sqrt(2)));
}
double Softplus(double x) {
    return Log(1 + Exp(x));
}

// Inner Functions
double Exp(double x) {
    return std::exp(x);
}
double Inv(double x) {
    return 1 / x;
}
double Abs(double x) {
    return std::abs(x);
}
double Erf(double x) {
    return std::erf(x);
}
double Log(double x) {
    return std::log(x);
}

// Comparison
double Compare(double x, double y) {
    if (x==y) return 0.5;
    else return x > y ? 1.0 : 0.0;
}

/////////////////////////////////////////////////
//////          Removed+Optimized          //////
/////////////////////////////////////////////////

double _Tanh(double x) {
    double t = expm1(2*x);
    t = t + 2;
    // double normN = pow(10, ceil(log10(t)));
    // t = t / normN;
    // t = 1 / t;
    // t = -2 * t / normN;
    // return t;
    double z = 1 - 2/t;
    return z;
}

double _Inv(double x, int d) {
    double a=2-x, b=1-x;
    for (int i=0;i<d-1;i++) {
        b = b*b;
        a = a*(1+b);
    }
    return a;
}