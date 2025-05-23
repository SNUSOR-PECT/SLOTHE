#include <math.h>

double sigmoid (double x) {
    return 1 / (1+exp(-x));
}

double _swish (double x) {
    return x * sigmoid(x);
}