/*
 * Ported from C#
 * Original Author: Forrest Smith (https://github.com/forrestthewoods)
 * https://github.com/forrestthewoods/lib_fts/blob/master/code/fts_ballistic_trajectory.cs
 */

#pragma once
#ifndef __PROJECTILE_MATH_HPP__
#define __PROJECTILE_MATH_HPP__

class ProjectileMath
{
public:
    static float BallisticRange(const float& speed, const float& gravity, const float& initialHeight);

    static int SolveBallisticArc(const Ogre::Vector3& projPos,
                                 const float& projSpeed,
                                 const Ogre::Vector3& target,
                                 const float& gravity,
                                 Ogre::Vector3* s0,
                                 Ogre::Vector3* s1);

    static int SolveBallisticArc(const Ogre::Vector3& projPos,
                                 const float& projSpeed,
                                 const Ogre::Vector3& targetPos,
                                 const Ogre::Vector3& targetVelocity,
                                 const float& gravity,
                                 Ogre::Vector3* s0,
                                 Ogre::Vector3* s1);

    static bool SolveBallisticArcLateral(const Ogre::Vector3& projPos,
                                         const float& lateralSpeed,
                                         const Ogre::Vector3& targetPos,
                                         const float& maxHeight,
                                         Ogre::Vector3* fireVelocity,
                                         float* gravity);

    static bool SolveBallisticArcLateral(const Ogre::Vector3& projPos,
                                         const float& lateralSpeed,
                                         const Ogre::Vector3& target,
                                         const Ogre::Vector3& targetVelocity,
                                         const float& maxHeightOffset,
                                         Ogre::Vector3* fireVelocity,
                                         float* gravity,
                                         Ogre::Vector3* impactPoint);

private:
    ProjectileMath()
    {
    }
};

#endif // __PROJECTILE_MATH_HPP__
