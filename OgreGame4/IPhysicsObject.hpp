#pragma once

class IPhysicsObject
{
public:
    virtual ~IPhysicsObject()
    {
    }

    virtual void SetLinearVelocity(const btVector3& velocity) = 0;
    virtual void SetGravity(const btVector3& acceleration) = 0;
};