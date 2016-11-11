/*
 * Interface for objects that can receive damage (-hp or something)
 *
 * Copyright (C) 2016 Zhen Zhi Lee
 * Written by Zhen Zhi Lee (leezhenzhi@gmail.com)
 * 
 * Objects that are likely to receive damage should implement
 * this interface.
 */

#pragma once
#ifndef __I_DAMAGEABLE_OBJECT_HPP__
#define __I_DAMAGEABLE_OBJECT_HPP__

class IDamageableObject
{
public:
    virtual ~IDamageableObject()
    {
    }

    virtual void ApplyDamage(const float& damage) = 0;
    virtual float TotalDamageReceived() = 0;
};

#endif // __I_DAMAGEABLE_OBJECT_HPP__