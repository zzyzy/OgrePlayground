/*
 * Interface for an object managed
 * by the pool manager.
 * 
 * Copyright (C) 2016 Zhen Zhi Lee
 * Written by Zhen Zhi Lee (leezhenzhi@gmail.com)
 * 
 * Objects that wants to be managed by the pool
 * manager must implement this interface.
 */

#pragma once
#ifndef __I_POOL_OBJECT_HPP__
#define __I_POOL_OBJECT_HPP__

class IPoolObject
{
public:
    virtual ~IPoolObject()
    {
    }

    // Allow the pool manager to update the pool object
    virtual void Update(const float& deltaTime) = 0;

    // Is this object disposable?
    virtual bool IsDisposable() const = 0;

    // Is this object disposed?
    virtual bool IsDisposed() const = 0;

    // Dispose this object, basically cleaning up
    // and release any unused memory/dangling pointers
    virtual void Dispose() = 0;
};

#endif // __I_POOL_OBJECT_HPP__
