#include "ProjectileMath.hpp"
#include "Math.hpp"
#include <cassert>

float ProjectileMath::BallisticRange(const float& speed, const float& gravity, const float& initialHeight)
{
    // Handling these cases is up to your project's coding standards
    assert(speed > 0 && gravity > 0 && initialHeight >= 0); // BallisticRange called with invalid data

    // Derivation
    //   (1) x = speed * time * cos O
    //   (2) y = initial_height + (speed * time * sin O) - (.5 * gravity*time*time)
    //   (3) via quadratic: t = (speed*sin O)/gravity + sqrt(speed*speed*sin O + 2*gravity*initial_height)/gravity    [ignore smaller root]
    //   (4) solution: range = x = (speed*cos O)/gravity * sqrt(speed*speed*sin O + 2*gravity*initial_height)    [plug t back into x=speed*time*cos O]
    float angle = 45 * Math::Deg2Rad; // no air resistence, so 45 degrees provides maximum range
    float cosAngle = cos(angle);
    float sinAngle = sin(angle);

    float range = (speed * cosAngle / gravity) * (speed * sinAngle + sqrt(speed * speed * sinAngle * sinAngle + 2 * gravity * initialHeight));
    return range;
}

int ProjectileMath::SolveBallisticArc(const Ogre::Vector3& projPos,
                                      const float& projSpeed,
                                      const Ogre::Vector3& target,
                                      const float& gravity,
                                      Ogre::Vector3* s0,
                                      Ogre::Vector3* s1)
{
    // Handling these cases is up to your project's coding standards
    assert(projPos != target && projSpeed > 0 && gravity > 0); // SolveBallisticArc called with invalid data

    // C# requires out variables be set
    *s0 = Ogre::Vector3::ZERO;
    *s1 = Ogre::Vector3::ZERO;

    // Derivation
    //   (1) x = v*t*cos O
    //   (2) y = v*t*sin O - .5*g*t^2
    // 
    //   (3) t = x/(cos O*v)                                        [solve t from (1)]
    //   (4) y = v*x*sin O/(cos O * v) - .5*g*x^2/(cos^2 O*v^2)     [plug t into y=...]
    //   (5) y = x*tan O - g*x^2/(2*v^2*cos^2 O)                    [reduce; cos/sin = tan]
    //   (6) y = x*tan O - (g*x^2/(2*v^2))*(1+tan^2 O)              [reduce; 1+tan O = 1/cos^2 O]
    //   (7) 0 = ((-g*x^2)/(2*v^2))*tan^2 O + x*tan O - (g*x^2)/(2*v^2) - y    [re-arrange]
    //   Quadratic! a*p^2 + b*p + c where p = tan O
    //
    //   (8) let gxv = -g*x*x/(2*v*v)
    //   (9) p = (-x +- sqrt(x*x - 4gxv*(gxv - y)))/2*gxv           [quadratic formula]
    //   (10) p = (v^2 +- sqrt(v^4 - g(g*x^2 + 2*y*v^2)))/gx        [multiply top/bottom by -2*v*v/x; move 4*v^4/x^2 into root]
    //   (11) O = atan(p)

    Ogre::Vector3 diff = target - projPos;
    Ogre::Vector3 diffXZ = Ogre::Vector3(diff.x, 0.0f, diff.z);
    float groundDist = diffXZ.length();

    float speed2 = projSpeed * projSpeed;
    float speed4 = projSpeed * projSpeed * projSpeed * projSpeed;
    float y = diff.y;
    float x = groundDist;
    float gx = gravity * x;

    float root = speed4 - gravity * (gravity * x * x + 2 * y * speed2);

    // No solution
    if (root < 0)
        return 0;

    root = sqrt(root);

    float lowAng = atan2(speed2 - root, gx);
    float highAng = atan2(speed2 + root, gx);
    int numSolutions = lowAng != highAng ? 2 : 1;

    Ogre::Vector3 groundDir = diffXZ.normalisedCopy();
    *s0 = groundDir * cos(lowAng) * projSpeed + Ogre::Vector3::UNIT_Y * sin(lowAng) * projSpeed;
    if (numSolutions > 1)
        *s1 = groundDir * cos(highAng) * projSpeed + Ogre::Vector3::UNIT_Y * sin(highAng) * projSpeed;

    return numSolutions;
}

int ProjectileMath::SolveBallisticArc(const Ogre::Vector3& projPos,
                                      const float& projSpeed,
                                      const Ogre::Vector3& targetPos,
                                      const Ogre::Vector3& targetVelocity,
                                      const float& gravity,
                                      Ogre::Vector3* s0,
                                      Ogre::Vector3* s1)
{
    // Initialize output parameters
    *s0 = Ogre::Vector3::ZERO;
    *s1 = Ogre::Vector3::ZERO;

    // Derivation 
    //
    //  For full derivation see: blog.forrestthewoods.com
    //  Here is an abbreviated version.
    //
    //  Four equations, four unknowns (solution.x, solution.y, solution.z, time):
    //
    //  (1) proj_pos.x + solution.x*time = target_pos.x + target_vel.x*time
    //  (2) proj_pos.y + solution.y*time + .5*G*t = target_pos.y + target_vel.y*time
    //  (3) proj_pos.z + solution.z*time = target_pos.z + target_vel.z*time
    //  (4) proj_speed^2 = solution.x^2 + solution.y^2 + solution.z^2
    //
    //  (5) Solve for solution.x and solution.z in equations (1) and (3)
    //  (6) Square solution.x and solution.z from (5)
    //  (7) Solve solution.y^2 by plugging (6) into (4)
    //  (8) Solve solution.y by rearranging (2)
    //  (9) Square (8)
    //  (10) Set (8) = (7). All solution.xyz terms should be gone. Only time remains.
    //  (11) Rearrange 10. It will be of the form a*^4 + b*t^3 + c*t^2 + d*t * e. This is a quartic.
    //  (12) Solve the quartic using SolveQuartic.
    //  (13) If there are no positive, real roots there is no solution.
    //  (14) Each positive, real root is one valid solution
    //  (15) Plug each time value into (1) (2) and (3) to calculate solution.xyz
    //  (16) The end.

    double G = gravity;

    double A = projPos.x;
    double B = projPos.y;
    double C = projPos.z;
    double M = targetPos.x;
    double N = targetPos.y;
    double O = targetPos.z;
    double P = targetVelocity.x;
    double Q = targetVelocity.y;
    double R = targetVelocity.z;
    double S = projSpeed;

    double H = M - A;
    double J = O - C;
    double K = N - B;
    double L = -.5f * G;

    // Quartic Coeffecients
    double c0 = L * L;
    double c1 = 2 * Q * L;
    double c2 = Q * Q + 2 * K * L - S * S + P * P + R * R;
    double c3 = 2 * K * Q + 2 * H * P + 2 * J * R;
    double c4 = K * K + H * H + J * J;

    // Solve quartic
    double times[4];
    int numTimes = Math::SolveQuartic(c0, c1, c2, c3, c4, &times[0], &times[1], &times[2], &times[3]);

    // Sort so faster collision is found first
    std::sort(times, times + 4);

    // Plug quartic solutions into base equations
    // There should never be more than 2 positive, real roots.
    Ogre::Vector3 solutions[2];
    int numSolutions = 0;

    for (int i = 0; i < numTimes && numSolutions < 2; ++i)
    {
        double t = times[i];
        if (t <= 0)
            continue;

        solutions[numSolutions].x = static_cast<float>((H + P * t) / t);
        solutions[numSolutions].y = static_cast<float>((K + Q * t - L * t * t) / t);
        solutions[numSolutions].z = static_cast<float>((J + R * t) / t);
        ++numSolutions;
    }

    // Write out solutions
    if (numSolutions > 0) *s0 = solutions[0];
    if (numSolutions > 1) *s1 = solutions[1];

    return numSolutions;
}

bool ProjectileMath::SolveBallisticArcLateral(const Ogre::Vector3& projPos,
                                              const float& lateralSpeed,
                                              const Ogre::Vector3& targetPos,
                                              const float& maxHeight,
                                              Ogre::Vector3* fireVelocity,
                                              float* gravity)
{
    // Handling these cases is up to your project's coding standards
    assert(projPos != targetPos && lateralSpeed > 0 && maxHeight > projPos.y); // SolveBallisticArcLateral called with invalid data

    *fireVelocity = Ogre::Vector3::ZERO;
    *gravity = 0;

    Ogre::Vector3 diff = targetPos - projPos;
    Ogre::Vector3 diffXZ = Ogre::Vector3(diff.x, 0.0f, diff.z);
    float lateralDist = diffXZ.length();

    if (lateralDist == 0)
        return false;

    float time = lateralDist / lateralSpeed;

    *fireVelocity = diffXZ.normalisedCopy() * lateralSpeed;

    // System of equations. Hit max_height at t=.5*time. Hit target at t=time.
    //
    // peak = y0 + vertical_speed*halfTime + .5*gravity*halfTime^2
    // end = y0 + vertical_speed*time + .5*gravity*time^s
    // Wolfram Alpha: solve b = a + .5*v*t + .5*g*(.5*t)^2, c = a + vt + .5*g*t^2 for g, v
    float a = projPos.y; // initial
    float b = maxHeight; // peak
    float c = targetPos.y; // final

    *gravity = -4 * (a - 2 * b + c) / (time * time);
    (*fireVelocity).y = -(3 * a - 4 * b + c) / time;

    return true;
}

bool ProjectileMath::SolveBallisticArcLateral(const Ogre::Vector3& projPos,
                                              const float& lateralSpeed,
                                              const Ogre::Vector3& target,
                                              const Ogre::Vector3& targetVelocity,
                                              const float& maxHeightOffset,
                                              Ogre::Vector3* fireVelocity,
                                              float* gravity,
                                              Ogre::Vector3* impactPoint)
{
    // Handling these cases is up to your project's coding standards
    assert(projPos != target && lateralSpeed > 0); // SolveBallisticArcLateral called with invalid data

    // Initialize output variables
    *fireVelocity = Ogre::Vector3::ZERO;
    *gravity = 0.0f;
    *impactPoint = Ogre::Vector3::ZERO;

    // Ground plane terms
    Ogre::Vector3 targetVelXZ = Ogre::Vector3(targetVelocity.x, 0.0f, targetVelocity.z);
    Ogre::Vector3 diffXZ = target - projPos;
    diffXZ.y = 0;

    // Derivation
    //   (1) Base formula: |P + V*t| = S*t
    //   (2) Substitute variables: |diffXZ + targetVelXZ*t| = S*t
    //   (3) Square both sides: Dot(diffXZ,diffXZ) + 2*Dot(diffXZ, targetVelXZ)*t + Dot(targetVelXZ, targetVelXZ)*t^2 = S^2 * t^2
    //   (4) Quadratic: (Dot(targetVelXZ,targetVelXZ) - S^2)t^2 + (2*Dot(diffXZ, targetVelXZ))*t + Dot(diffXZ, diffXZ) = 0
    float c0 = targetVelXZ.dotProduct(targetVelXZ) - lateralSpeed * lateralSpeed;
    float c1 = 2.0f * diffXZ.dotProduct(targetVelXZ);
    float c2 = diffXZ.dotProduct(diffXZ);
    double t0, t1;
    int n = Math::SolveQuadric(c0, c1, c2, &t0, &t1);

    // pick smallest, positive time
    bool valid0 = n > 0 && t0 > 0;
    bool valid1 = n > 1 && t1 > 0;

    float t;
    if (!valid0 && !valid1)
        return false;
    else if (valid0 && valid1)
        t = std::min(static_cast<float>(t0), static_cast<float>(t1));
    else
        t = valid0 ? static_cast<float>(t0) : static_cast<float>(t1);

    // Calculate impact point
    *impactPoint = target + (targetVelocity * t);

    // Calculate fire velocity along XZ plane
    Ogre::Vector3 dir = *impactPoint - projPos;
    *fireVelocity = Ogre::Vector3(dir.x, 0.0f, dir.z).normalisedCopy() * lateralSpeed;

    // Solve system of equations. Hit max_height at t=.5*time. Hit target at t=time.
    //
    // peak = y0 + vertical_speed*halfTime + .5*gravity*halfTime^2
    // end = y0 + vertical_speed*time + .5*gravity*time^s
    // Wolfram Alpha: solve b = a + .5*v*t + .5*g*(.5*t)^2, c = a + vt + .5*g*t^2 for g, v
    float a = projPos.y; // initial
    float b = std::max(projPos.y, (*impactPoint).y) + maxHeightOffset; // peak
    float c = (*impactPoint).y; // final

    *gravity = -4 * (a - 2 * b + c) / (t * t);
    (*fireVelocity).y = -(3 * a - 4 * b + c) / t;

    return true;
}
