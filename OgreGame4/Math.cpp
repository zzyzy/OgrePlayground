#include "Math.hpp"

const float Math::PI = 22.0f / 7.0f;

const float Math::Rad2Deg = 360.0f / (PI * 2);

const float Math::Deg2Rad = (PI * 2) / 360.0f;

int Math::SolveQuadric(double c0,
                       double c1,
                       double c2,
                       double* s0,
                       double* s1)
{
    *s0 = 0;
    *s1 = 0;

    double p, q, D;

    /* normal form: x^2 + px + q = 0 */
    p = c1 / (2 * c0);
    q = c2 / c0;

    D = p * p - q;

    if (isZero(D))
    {
        *s0 = -p;
        return 1;
    }
    if (D < 0)
    {
        return 0;
    }
    /* if (D > 0) */
    double sqrt_D = sqrt(D);

    *s0 = sqrt_D - p;
    *s1 = -sqrt_D - p;
    return 2;
}

int Math::SolveCubic(double c0,
                     double c1,
                     double c2,
                     double c3,
                     double* s0,
                     double* s1,
                     double* s2)
{
    *s0 = 0;
    *s1 = 0;
    *s2 = 0;

    int num;
    double sub;
    double A, B, C;
    double sq_A, p, q;
    double cb_p, D;

    /* normal form: x^3 + Ax^2 + Bx + C = 0 */
    A = c1 / c0;
    B = c2 / c0;
    C = c3 / c0;

    /*  substitute x = y - A/3 to eliminate quadric term:  x^3 +px + q = 0 */
    sq_A = A * A;
    p = 1.0 / 3 * (-1.0 / 3 * sq_A + B);
    q = 1.0 / 2 * (2.0 / 27 * A * sq_A - 1.0 / 3 * A * B + C);

    /* use Cardano's formula */
    cb_p = p * p * p;
    D = q * q + cb_p;

    if (isZero(D))
    {
        if (isZero(q)) /* one triple solution */
        {
            *s0 = 0;
            num = 1;
        }
        else /* one single and one double solution */
        {
            double u = pow(-q, 1.0 / 3.0);
            *s0 = 2 * u;
            *s1 = -u;
            num = 2;
        }
    }
    else if (D < 0) /* Casus irreducibilis: three real solutions */
    {
        double phi = 1.0 / 3 * acos(-q / sqrt(-cb_p));
        double t = 2 * sqrt(-p);

        *s0 = t * cos(phi);
        *s1 = -t * cos(phi + PI / 3);
        *s2 = -t * cos(phi - PI / 3);
        num = 3;
    }
    else /* one real solution */
    {
        double sqrt_D = sqrt(D);
        double u = pow(sqrt_D - q, 1.0 / 3.0);
        double v = -pow(sqrt_D + q, 1.0 / 3.0);

        *s0 = u + v;
        num = 1;
    }

    /* resubstitute */
    sub = 1.0 / 3 * A;

    if (num > 0) *s0 -= sub;
    if (num > 1) *s1 -= sub;
    if (num > 2) *s2 -= sub;

    return num;
}

int Math::SolveQuartic(double c0,
                       double c1,
                       double c2,
                       double c3,
                       double c4,
                       double* s0,
                       double* s1,
                       double* s2,
                       double* s3)
{
    *s0 = 0;
    *s1 = 0;
    *s2 = 0;
    *s3 = 0;

    double coeffs[4];
    double z, u, v, sub;
    double A, B, C, D;
    double sq_A, p, q, r;
    int num;

    /* normal form: x^4 + Ax^3 + Bx^2 + Cx + D = 0 */
    A = c1 / c0;
    B = c2 / c0;
    C = c3 / c0;
    D = c4 / c0;

    /*  substitute x = y - A/4 to eliminate cubic term: x^4 + px^2 + qx + r = 0 */
    sq_A = A * A;
    p = -3.0 / 8 * sq_A + B;
    q = 1.0 / 8 * sq_A * A - 1.0 / 2 * A * B + C;
    r = -3.0 / 256 * sq_A * sq_A + 1.0 / 16 * sq_A * B - 1.0 / 4 * A * C + D;

    if (isZero(r))
    {
        /* no absolute term: y(y^3 + py + q) = 0 */

        coeffs[3] = q;
        coeffs[2] = p;
        coeffs[1] = 0;
        coeffs[0] = 1;

        num = SolveCubic(coeffs[0], coeffs[1], coeffs[2], coeffs[3], s0, s1, s2);
    }
    else
    {
        /* solve the resolvent cubic ... */
        coeffs[3] = 1.0 / 2 * r * p - 1.0 / 8 * q * q;
        coeffs[2] = -r;
        coeffs[1] = -1.0 / 2 * p;
        coeffs[0] = 1;

        SolveCubic(coeffs[0], coeffs[1], coeffs[2], coeffs[3], s0, s1, s2);

        /* ... and take the one real solution ... */
        z = *s0;

        /* ... to build two quadric equations */
        u = z * z - r;
        v = 2 * z - p;

        if (isZero(u))
            u = 0;
        else if (u > 0)
            u = sqrt(u);
        else
            return 0;

        if (isZero(v))
            v = 0;
        else if (v > 0)
            v = sqrt(v);
        else
            return 0;

        coeffs[2] = z - u;
        coeffs[1] = q < 0 ? -v : v;
        coeffs[0] = 1;

        num = SolveQuadric(coeffs[0], coeffs[1], coeffs[2], s0, s1);

        coeffs[2] = z + u;
        coeffs[1] = q < 0 ? v : -v;
        coeffs[0] = 1;

        if (num == 0) num += SolveQuadric(coeffs[0], coeffs[1], coeffs[2], s0, s1);
        if (num == 1) num += SolveQuadric(coeffs[0], coeffs[1], coeffs[2], s1, s2);
        if (num == 2) num += SolveQuadric(coeffs[0], coeffs[1], coeffs[2], s2, s3);
    }

    /* resubstitute */
    sub = 1.0 / 4 * A;

    if (num > 0) *s0 -= sub;
    if (num > 1) *s1 -= sub;
    if (num > 2) *s2 -= sub;
    if (num > 3) *s3 -= sub;

    return num;
}
