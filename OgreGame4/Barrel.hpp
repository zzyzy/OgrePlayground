#pragma once

#include "PoolManager.hpp"

class Barrel
{
public:
    Barrel(Ogre::SceneNode* barrel,
        Ogre::SceneManager* world, 
        PhysicsContext* physics,
        const size_t& maxPoolSize,
        const float& delayBetweenShots = 1.0f,
        const float& shellSpeed = 50.0f,
        const float& shellMass = 1.0f,
        const float& blastForce = 0.5f,
        const float& blastRadius = 3.0f);

    void Update(const float& deltaTime);
    bool IsReady() const;
    void FireAt(const Ogre::Vector3& target);

private:
    Ogre::SceneNode* mBarrel;
    Ogre::SceneManager* mWorld;
    PhysicsContext* mPhysics;
    PoolManager mPool;
    float mDelayBetweenShots;
    float mElapsedDelay;
    float mShellSpeed;
    float mShellMass;
    float mBlastForce;
    float mBlastRadius;
    Ogre::Vector3 mProjectileVelocity;
    float mProjectileGravity;
};
