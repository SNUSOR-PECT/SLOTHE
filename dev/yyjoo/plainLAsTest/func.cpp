#include "func.hpp"

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
    t = _Inv(t, 7);
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

double Inv(double x) {
    return 1/x;
}

double _Inv(double x, int d) {
    double a=2-x, b=1-x;
    for (int i=0;i<d-1;i++) {
        b = b*b;
        a = a*(1+b);
    }
    return a;
}