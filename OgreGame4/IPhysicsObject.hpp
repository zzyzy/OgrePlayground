/*
 * Interface for a physics object.
 * 
 * Copyright (C) 2016 Zhen Zhi Lee
 * Written by Zhen Zhi Lee (leezhenzhi@gmail.com)
 * 
 * Objects that interacts with the physics engine
 * should implement this interface.
 */

#pragma once
#ifndef __I_PHYSICS_OBJECT_HPP__
#define __I_PHYSICS_OBJECT_HPP__

class IPhysicsObject
{
public:
    virtual ~IPhysicsObject()
    {
    }

    virtual void SetLinearVelocity(const btVector3& velocity) = 0;

    virtual void SetGravity(const btVector3& acceleration) = 0;
};

#endif // __I_PHYSICS_OBJECT_HPP__
