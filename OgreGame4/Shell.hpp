#pragma once

#include "Projectile.hpp"
#include "IPoolObject.hpp"

class Shell : public Projectile, public IPoolObject
{
public:
    Shell(btRigidBody* rbody,
          Ogre::SceneManager* world,
          PhysicsContext* physics,
          const float& blastForce,
          const float& blastRadius,
          const float& blastDuration = 1.0f) :
        Projectile(rbody, physics),
        mWorld(world),
        mBlastForce(blastForce),
        mBlastRadius(blastRadius),
        mBlastDuration(blastDuration),
        mBlastTimer(0.0f),
        mDisposed(false),
        mBlastCollider(nullptr),
        mParticleSystem(nullptr),
        mParticleNode(nullptr)
    {
        assert(world != nullptr);
    }

    Shell(const Shell& shell) :
        Projectile(shell.mRBody, shell.mPhysics),
        mWorld(shell.mWorld),
        mBlastForce(shell.mBlastForce),
        mBlastRadius(shell.mBlastRadius),
        mBlastDuration(shell.mBlastDuration),
        mBlastTimer(0.0f),
        mDisposed(false),
        mBlastCollider(nullptr),
        mParticleSystem(nullptr),
        mParticleNode(nullptr)
    {
    }

    void operator=(const Shell& shell)
    {
        mRBody = shell.mRBody;
        mPhysics = shell.mPhysics;
        mWorld = shell.mWorld;
        mBlastForce = shell.mBlastForce;
        mBlastRadius = shell.mBlastRadius;
        mBlastDuration = shell.mBlastDuration;
        mBlastTimer = 0.0f;
        mDisposed = false;
        mBlastCollider = nullptr;
        mParticleSystem = nullptr;
        mParticleNode = nullptr;
    }

    void Update(const float& deltaTime) override
    {
        if (mBlastTimer >= mBlastDuration) return;

        if (mRBody)
        {
            if (mRBody->getCenterOfMassPosition().y() < 2.0f)
            {
                assert(mBlastCollider == nullptr);
                btTransform startTrans = mRBody->getCenterOfMassTransform();
                btCollisionShape* shape = new btSphereShape(mBlastRadius);
                mBlastCollider = mPhysics->CreateGhostObject(startTrans, shape, btBroadphaseProxy::DefaultFilter, btBroadphaseProxy::DefaultFilter);
                mBlastCollider->setCollisionFlags(mBlastCollider->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

                // create a particle system named explosions using the explosionTemplate
                std::stringstream stream;
                stream << this << "_Explosion";
                auto name = stream.str();
                mParticleSystem = mWorld->createParticleSystem(name, "explosionTemplate");
                // attach the particle system to a scene node
                mParticleNode = mWorld->getRootSceneNode()->createChildSceneNode();
                mParticleNode->attachObject(mParticleSystem);
                mParticleNode->setPosition(Convert(mRBody->getCenterOfMassPosition()));
                mParticleNode->translate(0, 2, 0);
                // fast forward to the point where the particle has been emitted
                mParticleSystem->fastForward(4.6);

                auto node = static_cast<PhysicsContext::MotionState*>(mRBody->getMotionState())->GetNode();
                auto entity = static_cast<Ogre::Entity*>(node->getAttachedObject(0));
                node->detachAllObjects();
                mWorld->destroyEntity(entity);
                mWorld->destroySceneNode(node);
                mPhysics->DestroyRigidBody(mRBody);
                mRBody = nullptr;
            }
        }
        else
        {
            auto collidedObjects = mPhysics->GetAllCollidedObjects(mBlastCollider);

            for (auto it = collidedObjects.begin(); it != collidedObjects.end(); ++it)
            {
                auto rayFromWorld = mBlastCollider->getWorldTransform().getOrigin();
                auto rayToWorld = static_cast<btRigidBody*>(*it)->getCenterOfMassPosition();
                auto impulse = rayToWorld - rayFromWorld;

                impulse.normalize();
                impulse *= mBlastForce;

                mPhysics->ShootRay(rayFromWorld, rayToWorld, impulse);
            }

            mBlastTimer += deltaTime;
        }
    }

    bool IsDisposable() const override
    {
        return mBlastTimer >= mBlastDuration;
    }

    bool IsDisposed() const override
    {
        return mDisposed;
    }

    void Dispose() override
    {
        mPhysics->DestroyGhostObject(mBlastCollider);
        mParticleNode->detachAllObjects();
        mWorld->destroyParticleSystem(mParticleSystem);
        mWorld->destroySceneNode(mParticleNode);
        mWorld = nullptr;
        mBlastCollider = nullptr;
        mParticleSystem = nullptr;
        mParticleNode = nullptr;
        mDisposed = true;
    }

private:
    Ogre::SceneManager* mWorld;
    float mBlastForce;
    float mBlastRadius;
    float mBlastDuration;
    float mBlastTimer;
    bool mDisposed;
    btPairCachingGhostObject* mBlastCollider;
    Ogre::ParticleSystem* mParticleSystem;
    Ogre::SceneNode* mParticleNode;
};
