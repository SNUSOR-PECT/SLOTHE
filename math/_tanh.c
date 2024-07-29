#include <stdlib.h>
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

static const double one = 1.0, two = 2.0, tiny = 1.0e-300;

double _tanh (double x) {
	double t, z;
	int32_t jx, ix, lx;

	/* High word of |x|. */
	EXTRACT_WORDS (jx, lx, x);
	ix = jx & 0x7fffffff;

	/* x if INF of NaN */
	if (ix >= 0x7ff00000)
	{
		if (jx >= 0)
			return one / x + one; 		/* tanh(+-inf)=+-1 */
		else
			return one / x - one;		/* tanh(NaN) = NaN */
	}

	/* |x| < 22 */
	if (ix < 0x40360000)		/* |x| < 22 */
	{
		if ((ix | lx) == 0)
			return x;	/* x == +-0 */
		if (ix < 0x3c800000)	/* |x| < 2**-55 */
		{
			// math_check_force_underflow (x);
			return x * (one + x);		/* tanh(small) = small */
		}
		if (ix >= 0x3ff00000) 	/* |x| >= 1 */
		{
			t = expm1(two * fabs(x));
			z = one - two / (t + two);	
		}
		else
		{
			t = expm1(-two * fabs(x));
			z = -t / (t + two);
		}
		/* |x| > 22, return +-1 */
	}
	else
		z = one - tiny;		/* raised inexact flag */

	return (jx >= 0) ? z : -z;
}

int main(void) {
	_tanh(2.0f);
}
