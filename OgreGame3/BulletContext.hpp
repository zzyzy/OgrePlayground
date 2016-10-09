//
// OgreGame3/BulletContext.hpp
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

#include "BulletCollision/CollisionDispatch/btGhostObject.h"

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
	class MotionState : public btMotionState
	{
	public:
		MotionState(const btTransform& initialPosition,
		            Ogre::SceneNode* node) :
			mPosition(initialPosition),
			mSubject(node)
		{
		}

		void SetNode(Ogre::SceneNode* node)
		{
			mSubject = node;
		}

		Ogre::SceneNode* GetNode() const
		{
			return mSubject;
		}

		void getWorldTransform(btTransform& worldTrans) const override
		{
			worldTrans = mPosition;
		}

		void setWorldTransform(const btTransform& worldTrans) override
		{
			auto rotation = worldTrans.getRotation();
			auto position = worldTrans.getOrigin();
			mSubject->setOrientation(convert(rotation));
			mSubject->setPosition(convert(position));
		}

	private:
		btTransform mPosition;
		Ogre::SceneNode* mSubject;
	};

	BulletContext();
	~BulletContext();

	// Basic lifecycle
	void Setup();
	void Teardown();
	void Update(const float& deltaTime) const;

	// RigidBody
	btRigidBody* CreateRigidBody(const float& mass, const btTransform& startTrans, btCollisionShape* collisionShape, Ogre::SceneNode* node);
	void DestroyRigidBody(btRigidBody* rigidBody);

	// GhostObject
	btPairCachingGhostObject* BulletContext::CreateGhostObject(const btTransform& startTrans,
	                                                           btCollisionShape* collisionShape,
	                                                           btBroadphaseProxy::CollisionFilterGroups filterGroup = btBroadphaseProxy::AllFilter,
	                                                           btBroadphaseProxy::CollisionFilterGroups filterMask = btBroadphaseProxy::AllFilter);
	void DestroyGhostObject(btPairCachingGhostObject* ghostObject);

	// RigidBody collision detection
	btCollisionObject* GetCollidedObject(btCollisionObject* object) const;
	std::set<btCollisionObject*> GetAllCollidedObjects(btCollisionObject* object) const;

	// GhostObject collision detection
	std::set<btCollisionObject*> GetAllCollidedObjects(btPairCachingGhostObject* object) const;

	// Shoot ray and apply impulse
	// TODO Change to ShootImpulseRay
	void ShootRay(const btVector3& rayFromWorld, const btVector3& rayToWorld, const btVector3& impulse) const;

private:
	btDefaultCollisionConfiguration* mCollisionConfiguration;
	btCollisionDispatcher* mDispatcher;
	btBroadphaseInterface* mBroadphase;
	btSequentialImpulseConstraintSolver* mSolver;
	btAlignedObjectArray<btCollisionShape*> mCollisionShapes;
	btDiscreteDynamicsWorld* mDynamicsWorld;
};

#endif // #ifndef __BULLETCONTEXT_HPP__
