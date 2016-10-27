#pragma once

class IPoolObject
{
public:
    virtual ~IPoolObject()
    {
    }

    virtual void Update(const float& deltaTime) = 0;
    virtual bool IsDisposable() const = 0;
    virtual bool IsDisposed() const = 0;
    virtual void Dispose() = 0;
};