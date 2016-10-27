/*
 * Pool manager
 * 
 * Copyright (C) 2016 Zhen Zhi Lee
 * Written by Zhen Zhi Lee (leezhenzhi@gmail.com)
 * 
 * Manages a collection of objects that implements
 * the interface IPoolObject.
 */

#pragma once
#ifndef __POOL_MANAGER_HPP__
#define __POOL_MANAGER_HPP__

#include "PhysicsContext.hpp"
#include "IPoolObject.hpp"

class PoolManager
{
public:
    PoolManager(Ogre::SceneManager* world, PhysicsContext* physics);

    PoolManager(Ogre::SceneManager* world, PhysicsContext* physics, const size_t& maxPoolSize);

    ~PoolManager();

    void Update(const float& deltaTime);

    void Add(IPoolObject* object);

    void Remove(IPoolObject* object);

    size_t CurrentSize() const;

    size_t MaxSize() const;

private:
    Ogre::SceneManager* mWorld;
    PhysicsContext* mPhysics;
    size_t mMaxPoolSize;
    std::set<IPoolObject*> mPool;
};

#endif // __POOL_MANAGER_HPP__
