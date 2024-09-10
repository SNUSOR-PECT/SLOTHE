// ref: https://github.com/lattera/glibc/blob/master/sysdeps/ieee754/dbl-64/s_erf.c
#include "_acosh.hpp"

double _acosh1 (double x) {
    double t;
    int32_t hx;
    uint32_t lx;
    EXTRACT_WORDS (hx, lx, x);

    if (hx < 0x3ff00000) {                          /* x < 1 */
        return (x - x) / (x - x);
    } else if (hx >= 0x41b00000) {                  /* x > 2**28 */
        if (hx >= 0x7ff00000) {                     /* x is inf or NaN */
            return x+x;
        } else {
            return log(x) + ln2;          /* acosh(huge) = log(2x) */
        }
    } else if (((hx - 0x3ff00000) | lx) == 0) {     /* acosh(1) = 0*/
        return 0.0;
    } else if (hx > 0x40000000) {                   /* 2**28 > x > 2 */
        t = x*x;
        return log(2.0*x - one / (x + sqrt(t-one)));
    } else {                                        /* 1<x<2 */
        t = x - one;
        return log1p(t + sqrt(2.0*t + t*t));
    }
}

double _acosh2 (double x) {
    double t;
    int32_t hx;
    uint32_t lx;
    EXTRACT_WORDS (hx, lx, x);

    if (hx < 0x3ff00000) {                          /* x < 1 */
        return (x - x) / (x - x);
    } else if (hx >= 0x41b00000) {                  /* x > 2**28 */
        if (hx >= 0x7ff00000) {                     /* x is inf or NaN */
            return x+x;
        } else {
            return log(x) + ln2;          /* acosh(huge) = log(2x) */
        }
    } else if (((hx - 0x3ff00000) | lx) == 0) {     /* acosh(1) = 0*/
        return 0.0;
    } else if (hx > 0x40000000) {                   /* 2**28 > x > 2 */
        t = x*x;
        return log(2.0*x - one / (x + sqrt(t-one)));
    } else {                                        /* 1<x<2 */
        t = x - one;
        return log1p(t + sqrt(2.0*t + t*t));
    }
}