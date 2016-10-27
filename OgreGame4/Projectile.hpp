#pragma once

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
