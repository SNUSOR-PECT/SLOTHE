#include <math.h>

double _softplus (double x) {
    return log(1+exp(x));
}