// ref: https://github.com/tensorflow/tensorflow/blob/v2.16.1/tensorflow/python/ops/nn_impl.py#L433
#include <math.h>

double sigmoid (double x) {
    return 1 / (1+exp(-x));
}

double _swish (double x) {
    return x * sigmoid(x);
}