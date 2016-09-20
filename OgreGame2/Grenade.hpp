//
// OgreGame2/Grenade.hpp
// 
// TODO Add description
// http://gameprogrammingpatterns.com/state.html
//
// Author: Zhen Zhi Lee <leezhenzhi@gmail.com>
//
// Copyright (C) 2016 Zhen Zhi Lee.
//

#pragma once
#ifndef __GRENADE_HPP__
#define __GRENADE_HPP__

#include "Projectile.hpp"

class Grenade : public Projectile
{
public:
	Grenade(btRigidBody* body,
	        const float& blastForce,
	        const float& blastRadius,
	        const float& timeToLive = 3.0f,
	        const float& totalDecayTime = 3.0f) :
		Projectile(timeToLive, body, true, totalDecayTime),
		mBlastForce(blastForce),
		mBlastRadius(blastRadius),
		mSceneMgr(nullptr),
		mBulletContext(nullptr),
		mGhostObject(nullptr),
		mDummyNode(nullptr),
		mDummyEntity(nullptr)
	{
	}

	~Grenade()
	{
	}

	void cleanupAshes() const
	{
		if (mGhostObject != nullptr)
		{
			mSceneMgr->destroyEntity(mDummyEntity);
			mSceneMgr->destroySceneNode(mDummyNode);
			mBulletContext->DestroyGhostObject(mGhostObject);
		}
	}

	void update(const float& deltaTime) override
	{
		Projectile::update(deltaTime);

		if (mGhostObject != nullptr)
		{
			auto co = mBulletContext->GetAllCollidedObjects(mGhostObject);

			for (auto it = co.begin(); it != co.end(); ++it)
			{
				auto rayFromWorld = mGhostObject->getWorldTransform().getOrigin();
				auto rayToWorld = static_cast<btRigidBody*>(*it)->getCenterOfMassPosition();
				auto impulse = rayToWorld - rayFromWorld;

				impulse.normalize();
				impulse *= mBlastForce;

				mBulletContext->ShootRay(rayFromWorld, rayToWorld, impulse);
			}
		}
	}

	void OnDying(Ogre::SceneManager* sceneMgr, BulletContext* bulletContext) override
	{
		btTransform startTrans = mBody->getCenterOfMassTransform();
		btCollisionShape* shape = new btSphereShape(mBlastRadius);

		// TODO Use particles
		mDummyEntity = sceneMgr->createEntity("sphere.mesh");
		mDummyNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
		mDummyNode->attachObject(mDummyEntity);
		mDummyNode->setPosition(convert(startTrans.getOrigin()));
		mDummyNode->scale(0.01f * mBlastRadius, 0.01f * mBlastRadius, 0.01f * mBlastRadius);
		//mDummyNode->setVisible(false);

		mGhostObject = bulletContext->CreateGhostObject(startTrans, shape, btBroadphaseProxy::DefaultFilter, btBroadphaseProxy::DefaultFilter);
		mGhostObject->setCollisionFlags(mGhostObject->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

		mSceneMgr = sceneMgr;
		mBulletContext = bulletContext;
	}

private:
	float mBlastForce;
	float mBlastRadius;
	Ogre::SceneManager* mSceneMgr;
	BulletContext* mBulletContext;
	btPairCachingGhostObject* mGhostObject;
	Ogre::SceneNode* mDummyNode;
	Ogre::Entity* mDummyEntity;
};

#endif // #ifndef __GRENADE_HPP__
