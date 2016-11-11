#pragma once

class ITimedObject
{
public:
    virtual ~ITimedObject()
    {
    };

    virtual bool HasDied() const = 0;
    virtual bool IsDying() const = 0;
    virtual void OnDying(Ogre::SceneManager* world, PhysicsContext* physics) = 0;
    virtual void Destroy(Ogre::SceneManager* world, PhysicsContext* physics) = 0;
};
