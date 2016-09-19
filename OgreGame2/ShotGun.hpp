//
// OgreGame2/ShotGun.hpp
// 
// TODO Add description
// http://gameprogrammingpatterns.com/state.html
//
// Author: Zhen Zhi Lee <leezhenzhi@gmail.com>
//
// Copyright (C) 2016 Zhen Zhi Lee.
//

#pragma once
#ifndef __SHOTGUN_HPP__
#define __SHOTGUN_HPP__

#include "Weapon.hpp"
#include "BulletContext.hpp"
#include <random>

class ShotGun : public WeaponState
{
public:
	explicit ShotGun(Ogre::SceneManager* sceneMgr, Ogre::Camera* camera, BulletContext* bulletContext) :
		WeaponState(sceneMgr, camera, bulletContext),
		mDelayBetweenShots(2.0f),
		mElapsedDelay(mDelayBetweenShots)
	{
	}

	WeaponState* handleInput(Weapon& weapon, const Input& input) override
	{
		if (input == Input::MOUSEDOWN)
		{
			if (mElapsedDelay >= mDelayBetweenShots)
			{
				// Shoot 10 times with random accuracy
				for (auto i = 0; i < 10; ++i)
				{
					Shoot();
				}
				mElapsedDelay = 0.0f;
			}
		}
		return nullptr;
	}

	void update(Weapon& weapon, const float& deltaTime) override
	{
		if (mElapsedDelay < mDelayBetweenShots)
		{
			mElapsedDelay += deltaTime;
		}
	}

	void Shoot() const
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> dis(0.4f, 0.6f);

		Ogre::Ray mouseRay = mCamera->getCameraToViewportRay(dis(gen), dis(gen));

		// Get a point one unit from the mouse ray origin, in the direction of the ray
		Ogre::Vector3 destination = mouseRay.getPoint(1);

		// Calculate the direction for the linear velocity
		btVector3 impulse(
			destination.x - mouseRay.getOrigin().x,
			destination.y - mouseRay.getOrigin().y,
			destination.z - mouseRay.getOrigin().z);

		impulse.normalize();
		// Scale to appropriate velocity
		impulse *= 10.0f;

		// Shoot ray and apply impulse if hit
		mBulletContext->ShootRay(convert(mouseRay.getOrigin()), convert(mouseRay.getPoint(1000)), impulse);
	}

private:
	float mDelayBetweenShots;
	float mElapsedDelay;
};

#endif // #ifndef __SHOTGUN_HPP__
