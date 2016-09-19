//
// OgreGame2/Binoculars.hpp
// 
// TODO Add description
//
// Author: Zhen Zhi Lee <leezhenzhi@gmail.com>
//
// Copyright (C) 2016 Zhen Zhi Lee.
//

#pragma once
#ifndef __BINOCULARS_HPP__
#define __BINOCULARS_HPP__

#include "Weapon.hpp"
#include "BulletContext.hpp"

class Binoculars : public WeaponState
{
public:
	enum class ZoomLevel
	{
		NONE,
		LEVEL1,
		LEVEL2
	};

	explicit Binoculars(Ogre::SceneManager* sceneMgr,
	                    Ogre::Camera* camera,
	                    BulletContext* bulletContext,
	                    Ogre::Overlay* scopeOverlay,
	                    Ogre::Radian defaultFOV) :
		WeaponState(sceneMgr, camera, bulletContext),
		mZoomLevel(ZoomLevel::NONE),
		mZoomOverlay(scopeOverlay),
		mDefaultFOV(defaultFOV)
	{
	}

	~Binoculars()
	{
		mZoomOverlay->hide();
		mCamera->setFOVy(mDefaultFOV);
	}

	WeaponState* handleInput(Weapon& weapon, const Input& input) override
	{
		if (input == Input::RIGHTMOUSEDOWN)
		{
			Zoom();
		}
		return nullptr;
	}

	void update(Weapon& weapon, const float& deltaTime) override
	{
	}

	void Zoom()
	{
		if (mZoomLevel == ZoomLevel::NONE)
		{
			mZoomLevel = ZoomLevel::LEVEL1;
			mZoomOverlay->show();
			mCamera->setFOVy(Ogre::Degree(30.0f));
		}
		/*else if (mZoomLevel == ZoomLevel::LEVEL1)
		{
			mZoomLevel = ZoomLevel::LEVEL2;
			mCamera->setFOVy(Ogre::Degree(20.0f));
		}*/
		else if (mZoomLevel == ZoomLevel::LEVEL1)
		{
			mZoomLevel = ZoomLevel::NONE;
			mZoomOverlay->hide();
			mCamera->setFOVy(mDefaultFOV);
		}
	}

	std::string name() override
	{
		return "Binocular";
	}

	// TODO Use a base class called "ZoomableWeapon" instead of this ugly way
	int zoomLevel() const
	{
		if (mZoomLevel == ZoomLevel::NONE)
		{
			return 0;
		}
		return 1;
	}

private:
	ZoomLevel mZoomLevel;
	Ogre::Overlay* mZoomOverlay;
	Ogre::Radian mDefaultFOV;
};

#endif // #ifndef __BINOCULARS_HPP__
