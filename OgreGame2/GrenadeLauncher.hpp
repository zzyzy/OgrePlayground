//
// OgreGame2/GrenadeLauncher.hpp
// 
// TODO Add description
//
// Author: Zhen Zhi Lee <leezhenzhi@gmail.com>
//
// Copyright (C) 2016 Zhen Zhi Lee.
//

#pragma once
#ifndef __GRENADELAUNCHER_HPP__
#define __GRENADELAUNCHER_HPP__

#include "Weapon.hpp"
#include "BulletContext.hpp"
#include "Grenade.hpp"

class GrenadeLauncher : public WeaponState
{
public:
	explicit GrenadeLauncher(Ogre::SceneManager* sceneMgr,
	                         Ogre::Camera* camera,
	                         BulletContext* bulletContext,
	                         const float& delayBetweenShots = 1.0f,
	                         const float& launchForce = 30.0f,
	                         const float& grenadeMass = 1.0f,
	                         const float& blastForce = 50.0f,
	                         const float& blastRadius = 10.0f,
	                         const unsigned int& poolSize = 5) :
		WeaponState(sceneMgr, camera, bulletContext),
		mCharging(false),
		mChargeTime(0.0f),
		mDelayBetweenShots(delayBetweenShots),
		mElapsedDelay(mDelayBetweenShots),
		mLaunchForce(launchForce),
		mGrenadeMass(grenadeMass),
		mBlastForce(blastForce),
		mBlastRadius(blastRadius),
		mMaxPoolSize(poolSize)
	{
	}

	~GrenadeLauncher()
	{
		for (auto it = mPool.begin(); it != mPool.end(); ++it)
		{
			it->killMyself(mSceneMgr, mBulletContext);
			it->cleanupAshes();
		}
		mPool.clear();
	}

	WeaponState* handleInput(Weapon& weapon, const Input& input) override
	{
		if (input == Input::MOUSEDOWN)
		{
			mCharging = true;
		}
		else if (input == Input::MOUSEUP)
		{
			if (mElapsedDelay >= mDelayBetweenShots)
			{
				if (mPool.size() <= mMaxPoolSize)
				{
					Throw(mLaunchForce, mChargeTime, mGrenadeMass, mPool);
					mElapsedDelay = 0.0f;
				}
			}
			mCharging = false;
			mChargeTime = 0.0f;
		}
		return nullptr;
	}

	void update(Weapon& weapon, const float& deltaTime) override
	{
		if (mCharging && mChargeTime < 1)
		{
			mChargeTime += deltaTime;
		}

		if (mElapsedDelay < mDelayBetweenShots)
		{
			mElapsedDelay += deltaTime;
		}

		for (auto it = mPool.begin(); it != mPool.end(); ++it)
		{
			it->update(deltaTime);

			if (it->hasDied() && !it->isDecaying())
			{
				it->killMyself(mSceneMgr, mBulletContext);
			}
			else
			{
				if (it->hasDecayed())
				{
					it->cleanupAshes();
				}
			}
		}

		// TODO Maybe I can use two pools? One for non decay and decayed
		for (auto it = mPool.begin(); it != mPool.end();)
		{
			if (it->hasDecayed())
			{
				it = mPool.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	void Throw(const float& launchForce,
	           const float& chargeTime,
	           const float& grenadeMass,
	           std::vector<Grenade>& pool) const
	{
		// Get the mouse ray, i.e. ray from the mouse cursor 'into' the screen 
		Ogre::Ray mouseRay = mCamera->getCameraToViewportRay(0.5f, 0.5f);
		// Get a point one unit from the mouse ray origin, in the direction of the ray
		Ogre::Vector3 destination = mouseRay.getPoint(1);

		// Calculate the direction for the linear velocity
		btVector3 linearVelocity(
			destination.x - mouseRay.getOrigin().x,
			destination.y - mouseRay.getOrigin().y,
			destination.z - mouseRay.getOrigin().z);

		linearVelocity.normalize();
		// Scale to appropriate velocity
		linearVelocity *= launchForce * chargeTime;

		// TODO Import grenade model/mesh
		auto entity = mSceneMgr->createEntity("sphere.mesh");
		auto node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		entity->setMaterialName("Examples/Chrome");
		entity->setCastShadows(true);
		node->attachObject(entity);
		node->scale(0.01f, 0.01f, 0.01f);

		// Create a collision shape
		// Note that the size should match the size of the object that will be displayed
		btCollisionShape* collisionShape = new btBoxShape(btVector3(1, 1, 1));

		// The object's starting transformation
		btTransform startingTrans;
		startingTrans.setIdentity();
		startingTrans.setOrigin(convert(mouseRay.getOrigin()));
		startingTrans.setRotation(btQuaternion(0, 0, 0, 1));

		// Create the rigid body
		btRigidBody* rigidBody = mBulletContext->CreateRigidBody(grenadeMass, startingTrans, collisionShape, node);

		// Give the rigid body an initial velocity
		rigidBody->setLinearVelocity(linearVelocity);

		pool.push_back(Grenade(rigidBody, mBlastForce, mBlastRadius));
	}

	std::string name() override
	{
		return "Grenade";
	}

	float chargeTime() const
	{
		return mChargeTime;
	}

private:
	bool mCharging;
	float mChargeTime;
	float mDelayBetweenShots;
	float mElapsedDelay;
	float mLaunchForce;
	float mGrenadeMass;
	float mBlastForce;
	float mBlastRadius;
	unsigned int mMaxPoolSize;
	std::vector<Grenade> mPool;
};

#endif // #ifndef __GRENADELAUNCHER_HPP__
