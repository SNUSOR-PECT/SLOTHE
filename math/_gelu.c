// ref: https://github.com/tensorflow/tensorflow/blob/v2.16.1/tensorflow/python/ops/nn_ops.py#L3705
#include <math.h>

double _gelu (double x) {
    return 0.5 * x * (1 + erf(x / sqrt(2)));
}