#include <math.h>

double _sigmoid (double x) {
    return 1 / (1+exp(-x));
}