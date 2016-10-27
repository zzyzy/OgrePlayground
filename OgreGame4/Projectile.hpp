/*
 * Projectile class.
 * 
 * Copyright (C) 2016 Zhen Zhi Lee
 * Written by Zhen Zhi Lee (leezhenzhi@gmail.com)
 * 
 * Basic projectile class.
 */

#pragma once
#ifndef __PROJECTILE_HPP__
#define __PROJECTILE_HPP__

#include "IPhysicsObject.hpp"
#include "PhysicsContext.hpp"

class Projectile : public IPhysicsObject
{
public:
    explicit Projectile(btRigidBody* body,
                        PhysicsContext* physics) :
        mRBody(body),
        mPhysics(physics)
    {
        assert(body != nullptr && physics != nullptr);
    }

    virtual ~Projectile()
    {
    }

    void SetLinearVelocity(const btVector3& velocity) override
    {
        mRBody->setLinearVelocity(velocity);
    }

    void SetGravity(const btVector3& acceleration) override
    {
        mRBody->setGravity(acceleration);
    }

protected:
    btRigidBody* mRBody;
    PhysicsContext* mPhysics;
};

#endif // __PROJECTILE_HPP__
