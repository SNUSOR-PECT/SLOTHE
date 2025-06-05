// https://github.com/tensorflow/tensorflow/blob/master/tensorflow/core/kernels/relu_op_functor.h#L28
#include <math.h>
#include <algorithm>

double _relu (double x) {
    return std::max(x,0);
}