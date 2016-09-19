//
// OgreGame2/SniperRifle.hpp
// 
// TODO Add description
//
// Author: Zhen Zhi Lee <leezhenzhi@gmail.com>
//
// Copyright (C) 2016 Zhen Zhi Lee.
//

#pragma once
#ifndef __SNIPERRIFLE_HPP__
#define __SNIPERRIFLE_HPP__

#include "Weapon.hpp"
#include "BulletContext.hpp"

class SniperRifle : public WeaponState
{
public:
	enum class ZoomLevel
	{
		NONE,
		LEVEL1,
		LEVEL2
	};

	explicit SniperRifle(Ogre::SceneManager* sceneMgr,
	                     Ogre::Camera* camera,
	                     BulletContext* bulletContext,
						 Ogre::Overlay* scopeOverlay,
	                     const float& delayBetweenShots = 3.0f,
	                     const float& impactForce = 50.0f) :
		WeaponState(sceneMgr, camera, bulletContext),
		mDelayBetweenShots(delayBetweenShots),
		mElapsedDelay(mDelayBetweenShots),
		mImpactForce(impactForce),
		mZoomLevel(ZoomLevel::NONE),
		mScopeOverlay(scopeOverlay),
		mDefaultFOV()
	{
		mDefaultFOV = mCamera->getFOVy();
	}

	WeaponState* handleInput(Weapon& weapon, const Input& input) override
	{
		if (input == Input::MOUSEDOWN)
		{
			if (mElapsedDelay >= mDelayBetweenShots)
			{
				Shoot(mImpactForce);
				mElapsedDelay = 0.0f;
			}
		}
		else if (input == Input::RIGHTMOUSEDOWN)
		{
			Zoom();
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

	void Shoot(const float& impactForce) const
	{
		auto mouseRay = mCamera->getCameraToViewportRay(0.5f, 0.5f);

		// Get a point one unit from the mouse ray origin, in the direction of the ray
		auto destination = mouseRay.getPoint(1);

		// Calculate the direction for the linear velocity
		btVector3 impulse(
			destination.x - mouseRay.getOrigin().x,
			destination.y - mouseRay.getOrigin().y,
			destination.z - mouseRay.getOrigin().z);

		impulse.normalize();
		// Scale to appropriate velocity
		impulse *= impactForce;

		// Shoot ray and apply impulse if hit
		mBulletContext->ShootRay(convert(mouseRay.getOrigin()), convert(mouseRay.getPoint(1000)), impulse);
	}

	void Zoom()
	{
		if (mZoomLevel == ZoomLevel::NONE)
		{
			mZoomLevel = ZoomLevel::LEVEL1;
			mScopeOverlay->show();
			mCamera->setFOVy(Ogre::Degree(30.0f));
		} 
		else if (mZoomLevel == ZoomLevel::LEVEL1)
		{
			mZoomLevel = ZoomLevel::LEVEL2;
			mCamera->setFOVy(Ogre::Degree(20.0f));
		} else if (mZoomLevel == ZoomLevel::LEVEL2)
		{
			mZoomLevel = ZoomLevel::NONE;
			mScopeOverlay->hide();
			mCamera->setFOVy(mDefaultFOV);
		}
	}

	std::string name() override
	{
		return "Sniper Rifle";
	}

	// TODO Use a base class called "ZoomableWeapon" instead of this ugly way
	int zoomLevel() const
	{
		if (mZoomLevel == ZoomLevel::NONE)
		{
			return 0;
		} else if (mZoomLevel == ZoomLevel::LEVEL1)
		{
			return 1;
		}
		return 2;
	}

private:
	float mDelayBetweenShots;
	float mElapsedDelay;
	float mImpactForce;
	ZoomLevel mZoomLevel;
	Ogre::Overlay* mScopeOverlay;
	Ogre::Radian mDefaultFOV;
};

#endif // #ifndef __SNIPERRIFLE_HPP__
