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
    // double t = expm1(2*x);
    // t = t + 2;
    // double z = 1 - 2/t;
    // return z;

    double t = _Expm1(2*x);
    t = t + 2;
    // t = Inv(t);
    // double normN = pow(10, ceil(log10(t)));  // _Inv(t, 7) --> max_err=2.76599e-06
    double normN = 10e+4;   // _Inv(t, 20) --> max_err=5.56449e-05
    t = t / normN;
    t = _Inv(t, 20);
    t = t / normN;
    double z = 1 - 2 * t;
    return z;
}

// Estimated max error: 7.6274456637363524e-10
double innerExp(double x)
{
    double u = 7.4558560109173961e-13;
    u = u * x + 8.3526260566246237e-11;
    u = u * x + 7.9469351476292474e-09;
    u = u * x + 6.3575315810601785e-07;
    u = u * x + 4.0690111435471404e-05;
    u = u * x + 0.0019531253806492812;
    u = u * x + 0.062499999957691746;
    return u * x + 0.99999999923843008;
}

double _Exp(double x) {
    double t = innerExp(x);
    return pow(t, 16);
}

double _Expm1(double x) {
    return _Exp(x)-1;
}

double _Inv(double x, int d) {
    double a=2-x, b=1-x;
    for (int i=0;i<d-1;i++) {
        b = b*b;
        a = a*(1+b);
    }
    return a;
}