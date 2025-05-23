#include <math.h>

double _gelu (double x) {
    return x / 2 * (1 + erf(x / sqrt(2)));
}