/*
 * Copyright (C) 2016 Zhen Zhi Lee
 * Written by Zhen Zhi Lee (leezhenzhi@gmail.com)
 */

#include "Barrel.hpp"
#include "ProjectileMath.hpp"
#include "OgreEuler.hpp"
#include "Shell.hpp"

Barrel::Barrel(Ogre::SceneNode* turret,
               Ogre::SceneNode* barrel,
               Ogre::SceneNode* nozzle,
               Ogre::SceneManager* world,
               PhysicsContext* physics,
               const size_t& maxPoolSize,
               const float& delayBetweenShots,
               const float& shellSpeed,
               const float& shellMass,
               const float& blastForce,
               const float& blastRadius) :
    mTurret(turret),
    mBarrel(barrel),
    mNozzle(nozzle),
    mWorld(world),
    mPhysics(physics),
    mPool(maxPoolSize),
    mDelayBetweenShots(delayBetweenShots),
    mElapsedDelay(delayBetweenShots),
    mShellSpeed(shellSpeed),
    mShellMass(shellMass),
    mBlastForce(blastForce),
    mBlastRadius(blastRadius),
    mProjectileVelocity(Ogre::Vector3::ZERO),
    mProjectileGravity(0.0f)
{
    assert(turret != nullptr &&
        barrel != nullptr &&
        nozzle != nullptr &&
        world != nullptr &&
        physics != nullptr &&
        maxPoolSize > 0);
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
    angles.setPitch(mTurret->getOrientation().getPitch());
    auto turretRotation = angles;

    angles = Ogre::Euler(lookRotation);
    angles.setYaw(mBarrel->getOrientation().getYaw());
    angles.setRoll(mBarrel->getOrientation().getRoll());
    auto barrelRotation = angles;

    Ogre::Quaternion rotation = Ogre::Quaternion::Slerp(5.0f * deltaTime, mTurret->getOrientation(), turretRotation);
    mTurret->setOrientation(rotation);

    rotation = Ogre::Quaternion::Slerp(5.0f * deltaTime, mBarrel->getOrientation(), barrelRotation);
    mBarrel->setOrientation(rotation);

    // If barrel is in position, fire the projectile
    if (mTurret->getOrientation().equals(turretRotation, Ogre::Degree(1.0f)) &&
        mBarrel->getOrientation().equals(barrelRotation, Ogre::Degree(1.0f)) &&
        mPool.CurrentSize() < mPool.MaxSize() &&
        IsReady())
    {
        auto entity = mWorld->createEntity("sphere.mesh");
        auto node = mWorld->getRootSceneNode()->createChildSceneNode();
        auto shape = new btBoxShape(btVector3(1, 1, 1));
        auto position = mNozzle->_getDerivedPosition();
        btTransform transform;

        entity->setCastShadows(true);
        node->attachObject(entity);
        node->scale(0.01f, 0.01f, 0.01f);
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
        auto position = mNozzle->_getDerivedPosition();

        // Calculate the projectile velocity
        bool hasSolution = ProjectileMath::SolveBallisticArcLateral(position, mShellSpeed, target, position.y + 1.0f, &s0, &g);

        if (hasSolution)
        {
            mProjectileVelocity = s0;
            mProjectileGravity = g;
        }
    }
}
