/*
* SolveQuadric, SolveCubic, and SolveQuartic were ported from C as written for Graphics Gems I
* Original Author: Jochen Schwarze (schwarze@isa.de)
* https://github.com/erich666/GraphicsGems/blob/240a34f2ad3fa577ef57be74920db6c4b00605e4/gems/Roots3And4.c
*/

#pragma once
#ifndef __MATH_HPP__
#define __MATH_HPP__

class Math
{
public:
    /*
    * Solve quadratic equation: c0*x^2 + c1*x + c2. 
    * Returns number of solutions.
    */
    static int SolveQuadric(double c0,
                            double c1,
                            double c2,
                            double* s0,
                            double* s1);
    /*
    * Solve cubic equation: c0*x^3 + c1*x^2 + c2*x + c3. 
    * Returns number of solutions.
    */
    static int SolveCubic(double c0,
                          double c1,
                          double c2,
                          double c3,
                          double* s0,
                          double* s1,
                          double* s2);

    /*
     * Solve quartic function: c0*x^4 + c1*x^3 + c2*x^2 + c3*x + c4. 
     * Returns number of solutions.
     */
    static int SolveQuartic(double c0,
                            double c1,
                            double c2,
                            double c3,
                            double c4,
                            double* s0,
                            double* s1,
                            double* s2,
                            double* s3);

    static const float PI;

    static const float Rad2Deg;

    static const float Deg2Rad;

private:
    Math()
    {
    }

    /*
     * Utility function used by SolveQuadratic, SolveCubic, and SolveQuartic
     */
    static bool isZero(double d)
    {
        const double eps = 1e-9;
        return d > -eps && d < eps;
    }
};

#endif // __MATH_HPP__
