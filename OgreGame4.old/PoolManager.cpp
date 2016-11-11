/*
 * Copyright (C) 2016 Zhen Zhi Lee
 * Written by Zhen Zhi Lee (leezhenzhi@gmail.com)
 */

#include "PoolManager.hpp"

PoolManager::PoolManager(const size_t& maxPoolSize) :
    mMaxPoolSize(maxPoolSize)
{
    assert(maxPoolSize > 0);
}

PoolManager::~PoolManager()
{
    for (auto o : mPool)
    {
        if (o->IsDisposable())
        {
            o->Dispose();
            delete o;
        }
    }

    mPool.clear();
}

void PoolManager::Update(const float& deltaTime)
{
    for (auto o : mPool)
    {
        o->Update(deltaTime);

        if (o->IsDisposable())
        {
            o->Dispose();
        }
    }

    for (auto it = mPool.begin(); it != mPool.end();)
    {
        if ((*it)->IsDisposed())
        {
            delete (*it);
            it = mPool.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void PoolManager::Add(IPoolObject* object)
{
    assert(object != nullptr);
    mPool.insert(object);
}

void PoolManager::Remove(IPoolObject* object)
{
    assert(object != nullptr);
    delete object;
    mPool.erase(object);
}

size_t PoolManager::CurrentSize() const
{
    return mPool.size();
}

size_t PoolManager::MaxSize() const
{
    return mMaxPoolSize;
}
