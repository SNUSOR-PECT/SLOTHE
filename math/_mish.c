// ref: https://github.com/keras-team/keras/blob/v3.3.3/keras/src/activations/activations.py#L413
#include <math.h>

double softplus (double x) {
    return log(1+exp(x));
}

double _mish (double x) {
    return x * tanh(softplus(x));
}