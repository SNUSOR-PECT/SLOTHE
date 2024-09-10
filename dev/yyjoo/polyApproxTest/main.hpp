// ref: https://github.com/lattera/glibc/blob/master/sysdeps/ieee754/dbl-64/s_tanh.c

#include <stdlib.h>
#include <math.h>

// Degree 1 approximation of f(x) = exp(x/8) on interval [ -8, 8 ]
// Estimated max error: 2.7880158579550234e-1
double exp_inv_1(double x)
{
    double u = 0.14690014920547517;
    return u * x + 1.2642790490197415;
}
// Degree 2 approximation of f(x) = exp(x/8) on interval [ -8, 8 ]
// Estimated max error: 4.5017388402819014e-2
double exp_inv_2(double x)
{
    double u = 0.0086568891618262252;
    u = u * x + 0.14127297565512281;
    return u * x + 0.9890397284583653;
}
// Degree 3 approximation of f(x) = exp(x/8) on interval [ -8, 8 ]
// Estimated max error: 5.5283701086875885e-3
double exp_inv_3(double x)
{
    double u = 0.00035065133518781734;
    u = u * x + 0.0084839498184665855;
    u = u * x + 0.12445846375345487;
    return u * x + 0.99457947632469468;
}
// Degree 4 approximation of f(x) = exp(x/8) on interval [ -8, 8 ]
// Estimated max error: 5.4666760051379795e-4
double exp_inv_4(double x)
{
    double u = 1.0780155669648492e-05;
    u = u * x + 0.00034637748900164303;
    u = u * x + 0.0077942987045349363;
    u = u * x + 0.1246636564593058;
    return u * x + 1.0000900001021276;
}
// Degree 5 approximation of f(x) = exp(x/8) on interval [ -8, 8 ]
// Estimated max error: 4.5205511926115826e-5
double exp_inv_5(double x)
{
    double u = 2.6666842656053289e-07;
    u = u * x + 1.0691820403827191e-05;
    u = u * x + 0.00032504815651124286;
    u = u * x + 0.0077999528536713894;
    u = u * x + 0.12500479331356371;
    return u * x + 1.0000447502942726;
}
// Degree 6 approximation of f(x) = exp(x/8) on interval [ -8, 8 ]
// Estimated max error: 3.2108771033611466e-6
double exp_inv_6(double x)
{
    double u = 5.5147874420177263e-09;
    u = u * x + 2.6510006564821646e-07;
    u = u * x + 1.0162456168166248e-05;
    u = u * x + 0.00032517363418001693;
    u = u * x + 0.0078126553818993628;
    u = u * x + 0.12500278538942108;
    return u * x + 0.99999960146967537;
}
// Degree 7 approximation of f(x) = exp(x/8) on interval [ -8, 8 ]
// Estimated max error: 1.9982527697547409e-7
double exp_inv_7(double x)
{
    double u = 9.7946179621941935e-11;
    u = u * x + 5.4903884088928207e-09;
    u = u * x + 2.541307650672302e-07;
    u = u * x + 1.0164798576098333e-05;
    u = u * x + 0.00032552461957565293;
    u = u * x + 0.0078125991600794514;
    u = u * x + 0.1249999779356072;
    return u * x + 0.99999980139812239;
}
// Degree 8 approximation of f(x) = exp(x/8) on interval [ -8, 8 ]
// Estimated max error: 1.1064289311752762e-8
double exp_inv_8(double x)
{
    double u = 1.5240798495095524e-12;
    u = u * x + 9.7608458450598704e-11;
    u = u * x + 5.2953136375575388e-09;
    u = u * x + 2.5416859089170512e-07;
    u = u * x + 1.017260114989744e-05;
    u = u * x + 0.00032552340909871913;
    u = u * x + 0.0078124992951386744;
    u = u * x + 0.12499998762009649;
    return u * x + 1.0000000011013483;
}
// Degree 9 approximation of f(x) = exp(x/8) on interval [ -8, 8 ]
// Estimated max error: 5.5172466939350217e-10
double exp_inv_9(double x)
{
    double u = 2.1098698085983081e-14;
    u = u * x + 1.5198698902778767e-12;
    u = u * x + 9.4570322004214351e-11;
    u = u * x + 5.2958525224794877e-09;
    u = u * x + 2.5431441657319724e-07;
    u = u * x + 1.017257959393278e-05;
    u = u * x + 0.00032552081676327728;
    u = u * x + 0.0078124995710659208;
    u = u * x + 0.12500000006243503;
    return u * x + 1.0000000005494576;
}

// Degree 15 approximation of f(x) = exp(x)
// on interval [ -8, 8 ]
// p(x)=((((((((((((((2.1347933669654444e-12*x+3.2246970421738508e-11)*x-2.8462589448106964e-11)*x-4.1669566760104026e-10)*x+3.7958775994285677e-8)*x+4.2132604434418511e-7)*x+2.2652325774980805e-6)*x+2.0236501050563404e-5)*x+2.0891033993638475e-4)*x+1.4654247003862545e-3)*x+8.2141712465480412e-3)*x+4.1041720222194951e-2)*x+1.6727699688056654e-1)*x+5.019338538299158e-1)*x+9.9907996327451231e-1)*x+9.9902171452990445e-1
// Estimated max error: 1.082120710159185e-3
double g15(double x)
{
    double u = 2.1347933669654444e-12;
    u = u * x + 3.2246970421738508e-11;
    u = u * x + -2.8462589448106963e-11;
    u = u * x + -4.1669566760104027e-10;
    u = u * x + 3.7958775994285675e-08;
    u = u * x + 4.2132604434418509e-07;
    u = u * x + 2.2652325774980806e-06;
    u = u * x + 2.0236501050563403e-05;
    u = u * x + 0.00020891033993638476;
    u = u * x + 0.0014654247003862545;
    u = u * x + 0.0082141712465480417;
    u = u * x + 0.041041720222194954;
    u = u * x + 0.16727699688056655;
    u = u * x + 0.50193385382991584;
    u = u * x + 0.99907996327451232;
    return u * x + 0.9990217145299044;
}

// Estimated max error: 3.2870501857961531e-2
double tanh_15(double x)
{
    double u = -2.2642920914480758e-11;
    u = u * x + 0.0;
    u = u * x + 5.7259420120632315e-09;
    u = u * x + 0.0;
    u = u * x + -5.8930567915451826e-07;
    u = u * x + 0.0;
    u = u * x + 3.1748646806009152e-05;
    u = u * x + 0.0;
    u = u * x + -0.0009606227839164624;
    u = u * x + 0.0;
    u = u * x + 0.016368886821357127;
    u = u * x + 0.0;
    u = u * x + -0.15298707264416264;
    u = u * x + 0.0;
    u = u * x + 0.89573255135594454;
    return u * x + 0.0;
}

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

double _expm1 (double x) {
    double e = exp_inv_7(x);
    e = pow(e, 8); // exp(x)
    return e - 1.0;
}

double _tanh (double x) {
	double t, z;
	int32_t jx, ix, lx;

	/* High word of |x|. */
	EXTRACT_WORDS (jx, lx, x);
	ix = jx & 0x7fffffff;

	// /* x if INF of NaN */
	// if (ix >= 0x7ff00000)
	// {
	// 	if (jx >= 0)
	// 		return one / x + one; 		/* tanh(+-inf)=+-1 */
	// 	else
	// 		return one / x - one;		/* tanh(NaN) = NaN */
	// }

	/* |x| < 22 */
	if (ix < 0x40360000)		/* |x| < 22 */
	{
		// if ((ix | lx) == 0)
		// 	return x;	/* x == +-0 */
		// if (ix < 0x3c800000)	/* |x| < 2**-55 */
		// {
		// 	// math_check_force_underflow (x);
		// 	return x * (one + x);		/* tanh(small) = small */
		// }
		if (ix >= 0x3ff00000) 	/* |x| >= 1 */
		{
			t = _expm1(two * fabs(x));
			z = one - two / (t + two);	
		}
		else
		{
			t = _expm1(-two * fabs(x));
			z = -t / (t + two);
		}
		/* |x| > 22, return +-1 */
	}
	else
		z = one - tiny;		/* raised inexact flag */

	return (jx >= 0) ? z : -z;
}
