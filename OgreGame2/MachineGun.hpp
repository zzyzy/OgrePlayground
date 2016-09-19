//
// OgreGame2/HandGun.hpp
// 
// TODO Add description
// http://gameprogrammingpatterns.com/state.html
//
// Author: Zhen Zhi Lee <leezhenzhi@gmail.com>
//
// Copyright (C) 2016 Zhen Zhi Lee.
//

#pragma once
#ifndef __MACHINEGUN_HPP__
#define __MACHINEGUN_HPP__

#include "Weapon.hpp"
#include "BulletContext.hpp"
#include <random>

class MachineGun : public WeaponState
{
public:
	explicit MachineGun(Ogre::SceneManager* sceneMgr, Ogre::Camera* camera, BulletContext* bulletContext) :
		WeaponState(sceneMgr, camera, bulletContext),
		mDelayBetweenShots(0.25f),
		mElapsedDelay(mDelayBetweenShots),
		mIsShooting(false)
	{
	}

	WeaponState* handleInput(Weapon& weapon, const Input& input) override
	{
		if (input == Input::MOUSEDOWN)
		{
			mIsShooting = true;
		}
		else if (input == Input::MOUSEUP)
		{
			mIsShooting = false;
		}
		return nullptr;
	}

	void update(Weapon& weapon, const float& deltaTime) override
	{
		if (mElapsedDelay < mDelayBetweenShots)
		{
			mElapsedDelay += deltaTime;
		} else
		{
			if (!mIsShooting) return;
			Shoot();
			mElapsedDelay = 0.0f;
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

	std::string name() override
	{
		return "Machine Gun";
	}

private:
	float mDelayBetweenShots;
	float mElapsedDelay;
	bool mIsShooting;
};

#endif // #ifndef __MACHINEGUN_HPP__
