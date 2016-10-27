/*
 * Copyright (C) 2016 Zhen Zhi Lee
 * Written by Zhen Zhi Lee (leezhenzhi@gmail.com)
 */

#include "Barrel.hpp"
#include "ProjectileMath.hpp"
#include "OgreEuler.hpp"
#include "Shell.hpp"

Barrel::Barrel(Ogre::SceneNode* barrel,
               Ogre::SceneManager* world,
               PhysicsContext* physics,
               const size_t& maxPoolSize,
               const float& delayBetweenShots,
               const float& shellSpeed,
               const float& shellMass,
               const float& blastForce,
               const float& blastRadius) :
    mBarrel(barrel),
    mWorld(world),
    mPhysics(physics),
    mPool(world, physics, maxPoolSize),
    mDelayBetweenShots(delayBetweenShots),
    mElapsedDelay(delayBetweenShots),
    mShellSpeed(shellSpeed),
    mShellMass(shellMass),
    mBlastForce(blastForce),
    mBlastRadius(blastRadius),
    mProjectileVelocity(Ogre::Vector3::ZERO),
    mProjectileGravity(0.0f)
{
    assert(barrel != nullptr && world != nullptr && physics != nullptr && maxPoolSize > 0);
}

void Barrel::Update(const float& deltaTime)
{
    if (mElapsedDelay < mDelayBetweenShots)
    {
        mElapsedDelay += deltaTime;
    }

    mPool.Update(deltaTime);

    // If there's no calculated projectile velocity, skip the rest of the operation
    if (mProjectileVelocity == Ogre::Vector3::ZERO) return;

    // Only adjust the pitch of the barrel
    Ogre::Quaternion lookRotation = LookRotation(mProjectileVelocity.normalisedCopy(), Ogre::Vector3::UNIT_Y);
    Ogre::Euler angles = Ogre::Euler(lookRotation);
    angles.setYaw(mBarrel->getOrientation().getYaw());
    angles.setRoll(mBarrel->getOrientation().getRoll());
    lookRotation = angles;
    Ogre::Quaternion rotation = Ogre::Quaternion::Slerp(5.0f * deltaTime, mBarrel->getOrientation(), lookRotation);
    mBarrel->setOrientation(rotation);

    // If barrel is in position, fire the projectile
    if (mBarrel->getOrientation().equals(lookRotation, Ogre::Degree(1.0f)) &&
        mPool.CurrentSize() < mPool.MaxSize() &&
        IsReady())
    {
        auto entity = mWorld->createEntity("sphere.mesh");
        auto node = mWorld->getRootSceneNode()->createChildSceneNode();
        auto shape = new btBoxShape(btVector3(2, 2, 2));
        auto position = mBarrel->getPosition();
        btTransform transform;

        position.y += 2.0f;
        entity->setCastShadows(true);
        node->attachObject(entity);
        node->scale(0.02f, 0.02f, 0.02f);
        node->setPosition(position);
        transform.setIdentity();
        transform.setOrigin(Convert(node->getPosition()));

        auto rbody = mPhysics->CreateRigidBody(mShellMass, transform, shape, node);
        auto shell = new Shell(rbody, mWorld, mPhysics, mBlastForce, mBlastRadius);
        shell->SetLinearVelocity(Convert(mProjectileVelocity));
        shell->SetGravity(btVector3(0, -mProjectileGravity, 0));
        mPool.Add(shell);

        mElapsedDelay = 0.0f;
        mProjectileVelocity = Ogre::Vector3::ZERO;
        mProjectileGravity = 0.0f;
    }
}

bool Barrel::IsReady() const
{
    return mElapsedDelay >= mDelayBetweenShots;
}

void Barrel::FireAt(const Ogre::Vector3& target)
{
    if (IsReady())
    {
        Ogre::Vector3 s0;
        float g;
        auto position = mBarrel->getPosition();
        position.y += 2.0f;

        // Calculate the projectile velocity
        bool hasSolution = ProjectileMath::SolveBallisticArcLateral(position, mShellSpeed, target, 10.0f, &s0, &g);

        if (hasSolution)
        {
            mProjectileVelocity = s0;
            mProjectileGravity = g;
        }
    }
}
