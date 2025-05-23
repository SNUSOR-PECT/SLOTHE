#include <math.h>

double _relu (double x) {
    return x * (0.5 + 0.5 * signbit(x));
}