//
// OgreGame2/BallShooter.hpp
// 
// TODO Add description
// http://gameprogrammingpatterns.com/state.html
//
// Author: Zhen Zhi Lee <leezhenzhi@gmail.com>
//
// Copyright (C) 2016 Zhen Zhi Lee.
//

#pragma once
#ifndef __BALLSHOOTER_HPP__
#define __BALLSHOOTER_HPP__

#include "Weapon.hpp"
#include "Projectile.hpp"

class BallShooter : public WeaponState
{
public:
	explicit BallShooter(Ogre::SceneManager* sceneMgr, Ogre::Camera* camera, BulletContext* bulletContext) :
		WeaponState(sceneMgr, camera, bulletContext),
		mCharging(false),
		mChargeTime(0),
		mElapsedDelay(3.0f),
		mBoxCount(0),
		mPoolSize(5)
	{
	}

	~BallShooter()
	{
		for (auto it = mBallPool.begin(); it != mBallPool.end(); ++it)
		{
			it->second.killMyself(mSceneMgr, mBulletContext);
		}
		mBallPool.clear();
	}

	WeaponState* handleInput(Weapon& weapon, const Input& input) override
	{
		if (input == Input::MOUSEDOWN)
		{
			mCharging = true;
		}
		else if (input == Input::MOUSEUP)
		{

			if (mElapsedDelay >= 1)
			{
				Shoot();
				mElapsedDelay = 0;
			}

			mCharging = false;
			mChargeTime = 0;
		}

		return nullptr;
	}

	void update(Weapon& weapon, const float& deltaTime) override
	{
		if (mCharging)
		{
			if (mChargeTime < 1)
			{
				mChargeTime += deltaTime;
			}
		}

		if (mElapsedDelay < 1)
		{
			mElapsedDelay += deltaTime;
		}

		for (auto it = mBallPool.begin(); it != mBallPool.end();)
		{
			it->second.update(deltaTime);
			if (it->second.hasDied())
			{
				it->second.killMyself(mSceneMgr, mBulletContext);
				it = mBallPool.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	void Shoot()
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
		linearVelocity *= 30.0f * mChargeTime;

		// Create and shoot the box
		auto position = convert(mouseRay.getOrigin());
		auto orientation = btQuaternion(0, 0, 0, 1);

		// Create unique name
		std::ostringstream oss;
		oss << mBoxCount;
		std::string entityName = "Cube" + oss.str();
		// Increment box count
		mBoxCount++;

		// Create cube mesh with unique name
		Ogre::Entity* cube = mSceneMgr->createEntity(entityName, "sphere.mesh");
		cube->setMaterialName("Examples/SphereMappedRustySteel");
		cube->setCastShadows(true);
		Ogre::SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		node->attachObject(cube);
		// Scale it to appropriate size
		node->scale(0.01f, 0.01f, 0.01f);

		// Create a collision shape
		// Note that the size should match the size of the object that will be displayed
		btCollisionShape* collisionShape = new btBoxShape(btVector3(1, 1, 1));

		// The object's starting transformation
		btTransform startingTrans;
		startingTrans.setIdentity();
		startingTrans.setOrigin(position);
		startingTrans.setRotation(orientation);

		// Create the rigid body
		btRigidBody* rigidBody = mBulletContext->CreateRigidBody(1.0f, startingTrans, collisionShape, node);

		// Give the rigid body an initial velocity
		rigidBody->setLinearVelocity(linearVelocity);

		Projectile projectile(5.0f, rigidBody);
		mBallPool.insert(std::make_pair(entityName, projectile));
	}

	std::string name() override
	{
		return "Ball Shooter";
	}

	float chargeTime() const
	{
		return mChargeTime;
	}

private:
	bool mCharging;
	float mChargeTime;
	float mElapsedDelay;
	int mBoxCount;
	int mPoolSize;
	std::map<std::string, Projectile> mBallPool;
};

#endif // #ifndef __BALLSHOOTER_HPP__
