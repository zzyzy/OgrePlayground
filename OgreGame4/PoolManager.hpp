#pragma once

#include "PhysicsContext.hpp"
#include "IPoolObject.hpp"

class PoolManager
{
public:
    PoolManager(Ogre::SceneManager* world, PhysicsContext* physics);
    PoolManager(Ogre::SceneManager* world, PhysicsContext* physics, size_t maxPoolSize);
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
