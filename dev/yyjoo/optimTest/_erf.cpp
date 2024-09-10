// ref: https://github.com/lattera/glibc/blob/master/sysdeps/ieee754/dbl-64/s_erf.c
#include "_erf.hpp"

double _erf1 (double x) {
    int32_t hx, ix, i;
    double R, S, P, Q, s, y, z, r;
    GET_HIGH_WORD (hx, x);
    ix = hx & 0x7fffffff;

    // if (ix < 0x3feb0000) {                      /* |x| < 0.84375 */
        // double r1, r2, s1, s2, s3, z2, z4;
        // if (ix < 0x3e300000) {                  /* |x| < 2**-28 */
        //     if (ix < 0x00800000) {
        //         /* Avoid spurious underflow */
        //         double ret = 0.0625 * (16.0 * x + (16.0 * efx) * x);
        //         // math_check_force_underflow (ret);
        //         return ret;
        //     }
        //     return x + efx*x;
        // }
        // z = x * x;
        // r1 = pp[0] + z * pp[1]; z2 = z * z;
        // r2 = pp[2] + z * pp[3]; z4 = z2 * z2;
        // s1 = one + z * qq[1];
        // s2 = qq[2] + z * qq[3];
        // s3 = qq[4] + z * qq[5];
        // r = r1 + z2 * r2 + z4 * pp[4];
        // s = s1 + z2 * s2 + z4 * s3;
        // y = r / s;
        // return x + x * y;
    // }
    
    // if (ix < 0x3ff40000) {                     /* 0.84375 <= |x| < 1.25 */
        // double s2, s4, s6, P1, P2, P3, P4, Q1, Q2, Q3, Q4;

        // s = fabs (x) - one;
        // P1 = pa[0] + s * pa[1]; s2 = s * s;
        // Q1 = one + s * qa[1];   s4 = s2 * s2;
        // P2 = pa[2] + s * pa[3]; s6 = s4 * s2;
        // Q2 = qa[2] + s * qa[3];
        // P3 = pa[4] + s * pa[5];
        // Q3 = qa[4] + s * qa[5];
        // P4 = pa[6];
        // Q4 = qa[6];
        // P = P1 + s2 * P2 + s4 * P3 + s6 * P4;
        // Q = Q1 + s2 * Q2 + s4 * Q3 + s6 * Q4;

        // if (hx >= 0)
        //     return erx + P / Q;
        // else
        //     return -erx - P / Q;
    // }

    // if (ix >= 0x40180000) {                     /* inf > |x| >= 6 */
        // if (hx >= 0)
        //     return one - tiny;
        // else
        //     return tiny - one;
    // }

    x = fabs(x);
    s = one / (x*x);

    // if (ix < 0x4006DB6E) {                      /* |x| < 1/0.35 */
        double R1, R2, R3, R4, S1, S2, S3, S4, s2, s4, s6, s8;

        R1 = ra[0] + s * ra[1]; s2 = s * s;
        S1 = one + s * sa[1];  s4 = s2 * s2;
        R2 = ra[2] + s * ra[3]; s6 = s4 * s2;
        S2 = sa[2] + s * sa[3]; s8 = s4 * s4;
        R3 = ra[4] + s * ra[5];
        S3 = sa[4] + s * sa[5];
        R4 = ra[6] + s * ra[7];
        S4 = sa[6] + s * sa[7];
        R = R1 + s2 * R2 + s4 * R3 + s6 * R4;
        S = S1 + s2 * S2 + s4 * S3 + s6 * S4 + s8 * sa[8];
    // } 
    // else {                                    /* |x| >= 1/0.35 */
        // double R1, R2, R3, S1, S2, S3, S4, s2, s4, s6;

        // R1 = rb[0] + s * rb[1]; s2 = s * s;
        // S1 = one + s * sb[1];  s4 = s2 * s2;
        // R2 = rb[2] + s * rb[3]; s6 = s4 * s2;
        // S2 = sb[2] + s * sb[3];
        // R3 = rb[4] + s * rb[5];
        // S3 = sb[4] + s * sb[5];
        // S4 = sb[6] + s * sb[7];
        // R = R1 + s2 * R2 + s4 * R3 + s6 * rb[6];
        // S = S1 + s2 * S2 + s4 * S3 + s6 * S4;
    // }

    z = x;
    SET_LOW_WORD (z, 0);
    r = exp(-z*z - 0.5625) * exp((z - x) * (z+x) + R/S);

    if (hx >= 0)
        return one - r/x;
    else
        return r/x - one;
}

double _erf2 (double x) {
    int32_t hx, ix, i;
    double R, S, P, Q, s, y, z, r;
    GET_HIGH_WORD (hx, x);
    ix = hx & 0x7fffffff;

    // if (ix < 0x3feb0000) {                      /* |x| < 0.84375 */
    //     double r1, r2, s1, s2, s3, z2, z4;
        // if (ix < 0x3e300000) {                  /* |x| < 2**-28 */
        //     if (ix < 0x00800000) {
        //         /* Avoid spurious underflow */
        //         double ret = 0.0625 * (16.0 * x + (16.0 * efx) * x);
        //         // math_check_force_underflow (ret);
        //         return ret;
        //     }
        //     return x + efx*x;
        // }
    //     z = x * x;
    //     r1 = pp[0] + z * pp[1]; z2 = z * z;
    //     r2 = pp[2] + z * pp[3]; z4 = z2 * z2;
    //     s1 = one + z * qq[1];
    //     s2 = qq[2] + z * qq[3];
    //     s3 = qq[4] + z * qq[5];
    //     r = r1 + z2 * r2 + z4 * pp[4];
    //     s = s1 + z2 * s2 + z4 * s3;
    //     y = r / s;
    //     return x + x * y;
    // }
    
    // if (ix < 0x3ff40000) {                     /* 0.84375 <= |x| < 1.25 */
    //     double s2, s4, s6, P1, P2, P3, P4, Q1, Q2, Q3, Q4;

    //     s = fabs (x) - one;
    //     P1 = pa[0] + s * pa[1]; s2 = s * s;
    //     Q1 = one + s * qa[1];   s4 = s2 * s2;
    //     P2 = pa[2] + s * pa[3]; s6 = s4 * s2;
    //     Q2 = qa[2] + s * qa[3];
    //     P3 = pa[4] + s * pa[5];
    //     Q3 = qa[4] + s * qa[5];
    //     P4 = pa[6];
    //     Q4 = qa[6];
    //     P = P1 + s2 * P2 + s4 * P3 + s6 * P4;
    //     Q = Q1 + s2 * Q2 + s4 * Q3 + s6 * Q4;

    //     if (hx >= 0)
    //         return erx + P / Q;
    //     else
    //         return -erx - P / Q;
    // }

    // if (ix >= 0x40180000) {                     /* inf > |x| >= 6 */
    //     if (hx >= 0)
    //         return one - tiny;
    //     else
    //         return tiny - one;
    // }

    x = fabs(x);
    s = one / (x*x);

    // if (ix < 0x4006DB6E) {                      /* |x| < 1/0.35 */
        // double R1, R2, R3, R4, S1, S2, S3, S4, s2, s4, s6, s8;

        // R1 = ra[0] + s * ra[1]; s2 = s * s;
        // S1 = one + s * sa[1];  s4 = s2 * s2;
        // R2 = ra[2] + s * ra[3]; s6 = s4 * s2;
        // S2 = sa[2] + s * sa[3]; s8 = s4 * s4;
        // R3 = ra[4] + s * ra[5];
        // S3 = sa[4] + s * sa[5];
        // R4 = ra[6] + s * ra[7];
        // S4 = sa[6] + s * sa[7];
        // R = R1 + s2 * R2 + s4 * R3 + s6 * R4;
        // S = S1 + s2 * S2 + s4 * S3 + s6 * S4 + s8 * sa[8];
    // } 
    // else {                                    /* |x| >= 1/0.35 */
        double R1, R2, R3, S1, S2, S3, S4, s2, s4, s6;

        R1 = rb[0] + s * rb[1]; s2 = s * s;
        S1 = one + s * sb[1];  s4 = s2 * s2;
        R2 = rb[2] + s * rb[3]; s6 = s4 * s2;
        S2 = sb[2] + s * sb[3];
        R3 = rb[4] + s * rb[5];
        S3 = sb[4] + s * sb[5];
        S4 = sb[6] + s * sb[7];
        R = R1 + s2 * R2 + s4 * R3 + s6 * rb[6];
        S = S1 + s2 * S2 + s4 * S3 + s6 * S4;
    // }

    z = x;
    SET_LOW_WORD (z, 0);
    r = exp(-z*z - 0.5625) * exp((z - x) * (z+x) + R/S);

    if (hx >= 0)
        return one - r/x;
    else
        return r/x - one;
}

// ref: https://github.com/lattera/glibc/blob/master/sysdeps/ieee754/dbl-64/s_erf.c
#include "_erf.hpp"

double _erf_branch_eliminated (double x) {
    int32_t hx, ix, i;
    double R, S, P, Q, s, y, z, r;
    GET_HIGH_WORD (hx, x);
    ix = hx & 0x7fffffff;

     /* |x| < 0.84375 */
    if (ix < 0x3feb0000) {
        double r1, r2, s1, s2, s3, z2, z4;

        z = x * x;
        r1 = pp[0] + z * pp[1]; z2 = z * z;
        r2 = pp[2] + z * pp[3]; z4 = z2 * z2;
        s1 = one + z * qq[1];
        s2 = qq[2] + z * qq[3];
        s3 = qq[4] + z * qq[5];
        r = r1 + z2 * r2 + z4 * pp[4];
        s = s1 + z2 * s2 + z4 * s3;
        y = r / s;
        return x + x * y;
    }
    
    /* else */
    x = fabs(x);
    s = one / (x*x);

    double R1, R2, R3, R4, S1, S2, S3, S4, s2, s4, s6, s8;

    R1 = ra[0] + s * ra[1]; s2 = s * s;
    S1 = one + s * sa[1];  s4 = s2 * s2;
    R2 = ra[2] + s * ra[3]; s6 = s4 * s2;
    S2 = sa[2] + s * sa[3]; s8 = s4 * s4;
    R3 = ra[4] + s * ra[5];
    S3 = sa[4] + s * sa[5];
    R4 = ra[6] + s * ra[7];
    S4 = sa[6] + s * sa[7];
    R = R1 + s2 * R2 + s4 * R3 + s6 * R4;
    S = S1 + s2 * S2 + s4 * S3 + s6 * S4 + s8 * sa[8];

    z = x;
    SET_LOW_WORD (z, 0);
    r = exp(-z*z - 0.5625) * exp((z - x) * (z+x) + R/S);

    if (hx >= 0)
        return one - r/x;
    else
        return r/x - one;
}