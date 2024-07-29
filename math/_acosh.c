// ref: https://github.com/lattera/glibc/blob/master/sysdeps/ieee754/dbl-64/e_acosh.c

#include <stdint.h>
#include <math.h>

typedef union
{
	double value;
	struct
	{
		unsigned int lsw;
		unsigned int msw;
	} parts;
	unsigned long long word;
} ieee_double_shape_type;

#define EXTRACT_WORDS(ix0, ix1, d) \
	do { \
		ieee_double_shape_type ew_u; \
		ew_u.value = (d); \
		(ix0) = ew_u.parts.msw; \
		(ix1) = ew_u.parts.lsw; \
	} while (0)

static const double
    one = 1.0,
    ln2 = 6.93147180559945286227e-01;               /* 0x3FE62E42, 0xFEFA39EF */

double _acosh (double x) {
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
        return __log1p(t + sqrt(2.0*t + t*t));
    }
}

int main(void) {
	_acosh(0.5f);
}
