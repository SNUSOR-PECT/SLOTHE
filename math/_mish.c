#include <math.h>

double softplus (double x) {
    return log(1+exp(x));
}

double _mish (double x) {
    return x * tanh(softplus(x));
}