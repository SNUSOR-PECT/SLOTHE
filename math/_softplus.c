// ref: https://github.com/tensorflow/tensorflow/blob/master/tensorflow/core/kernels/softplus_op.h#L32
#include <math.h>

double _softplus (double x) {
    return log(1+exp(x));
}