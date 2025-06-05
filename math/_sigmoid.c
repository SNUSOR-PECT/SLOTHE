// ref: https://github.com/tensorflow/tensorflow/blob/master/tensorflow/core/kernels/cwise_ops.h#L905
// ref: https://gitlab.com/libeigen/eigen/-/blob/master/Eigen/src/Core/functors/UnaryFunctors.h#L1219
#include <math.h>

double _sigmoid (double x) {
    return 1 / (1+exp(-x));
}