//
// OgreGame2/BulletContext.hpp
// 
// TODO Add description
//
// Author: Zhen Zhi Lee <leezhenzhi@gmail.com>
//
// Copyright (C) 2016 Zhen Zhi Lee.
//

#pragma once
#ifndef __BULLETCONTEXT_HPP__
#define __BULLETCONTEXT_HPP__

inline Ogre::Vector3 convert(const btVector3& vec3)
{
    return Ogre::Vector3(vec3.x(), vec3.y(), vec3.z());
}

inline btVector3 convert(const Ogre::Vector3& vec3)
{
    return btVector3(vec3.x, vec3.y, vec3.z);
}

inline btQuaternion convert(const Ogre::Quaternion& Q)
{
    return btQuaternion(Q.x, Q.y, Q.z, Q.w);
}

inline Ogre::Quaternion convert(const btQuaternion& Q)
{
    return Ogre::Quaternion(Q.w(), Q.x(), Q.y(), Q.z());
}

class BulletContext
{
public:
    BulletContext();
    ~BulletContext();

    void Setup();
    void Teardown();
    void Update(float deltaTime) const;

    btRigidBody* CreateRigidBody(float mass, const btTransform& startTrans, btCollisionShape* collisionShape, Ogre::SceneNode* node);

private:
    class OgreMotionState : public btMotionState
    {
    public:
        OgreMotionState(const btTransform& initialPosition, Ogre::SceneNode* node) :
            mPosition(initialPosition), mSubject(node)
        {
        }

        ~OgreMotionState()
        {
        }

        void SetNode(Ogre::SceneNode* node)
        {
            mSubject = node;
        }

        void getWorldTransform(btTransform& worldTrans) const override
        {
            worldTrans = mPosition;
        }

        void setWorldTransform(const btTransform& worldTrans) override
        {
            auto rotation = worldTrans.getRotation();
            auto position = worldTrans.getOrigin();
            mSubject->setOrientation(rotation.w(), rotation.x(), rotation.y(), rotation.z());
            mSubject->setPosition(position.x(), position.y(), position.z());
        }

    private:
        btTransform mPosition;
        Ogre::SceneNode* mSubject;
    };

    btDefaultCollisionConfiguration* mCollisionConfiguration;
    btCollisionDispatcher* mDispatcher;
    btBroadphaseInterface* mBroadphase;
    btSequentialImpulseConstraintSolver* mSolver;
    btDiscreteDynamicsWorld* mDynamicsWorld;
    btAlignedObjectArray<btCollisionShape*> mCollisionShapes;
};

#endif // #ifndef __BULLETCONTEXT_HPP__
