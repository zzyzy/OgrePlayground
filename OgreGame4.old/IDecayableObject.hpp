#pragma once

class IDecayableObject
{
public:
    virtual ~IDecayableObject()
    {
    };

    virtual bool HasDecayed() const = 0;
    virtual bool IsDecaying() const = 0;
    virtual void OnDecay(Ogre::SceneManager* world, PhysicsContext* physics) = 0;
    virtual void Cleanup(Ogre::SceneManager* world, PhysicsContext* physics) = 0;
};
