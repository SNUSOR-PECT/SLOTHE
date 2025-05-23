#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

// NAFs
double Exp(double x) {
    return exp(x);
}
double Expm1(double x) {
    return expm1(x);
}
double Erf(double x) {
    return erf(x);
}
double Sigmoid(double x) {
    return 1 / (1+Exp(-x));
};
double Softplus(double x) {
    return log(1+Exp(x));
};
double Tanh(double x) {
    return tanh(x);
};
double Gelu(double x) {
    return x / 2 * (1 + erf(x / sqrt(2)));
};
double Swish(double x) {
    return x * Sigmoid(x);
};
double Mish(double x) {
    return x * Tanh(Softplus(x));
};
double Relu(double x) {
    return x > 0.0 ? x : 0.0;
};


#ifdef __cplusplus
}
#endif