//
// OgreGame2/WeaponState.hpp
// 
// TODO Add description
// http://gameprogrammingpatterns.com/state.html
//
// Author: Zhen Zhi Lee <leezhenzhi@gmail.com>
//
// Copyright (C) 2016 Zhen Zhi Lee.
//

#pragma once
#ifndef __WEAPONSTATE_HPP__
#define __WEAPONSTATE_HPP__

class Weapon;

class WeaponState
{
public:
	enum class Type
	{
		BALLSHOOTER,
		HANDGUN,
		MACHINEGUN,
		SHOTGUN,
		SNIPERRIFLE
	};

	enum class Input
	{
		MOUSEDOWN,
		MOUSEUP
	};

	WeaponState() :
		mSceneMgr(nullptr),
		mCamera(nullptr),
		mBulletContext(nullptr)
	{
	}

	explicit WeaponState(Ogre::SceneManager* sceneMgr, Ogre::Camera* camera, BulletContext* bulletContext) :
		mSceneMgr(sceneMgr),
		mCamera(camera),
		mBulletContext(bulletContext)
	{
	}

	virtual ~WeaponState()
	{
	}

	virtual WeaponState* handleInput(Weapon& weapon, const Input& input) = 0;
	virtual void update(Weapon& weapon, const float& deltaTime) = 0;

protected:
	Ogre::SceneManager* mSceneMgr;
	Ogre::Camera* mCamera;
	BulletContext* mBulletContext;
};

#endif // #ifndef __WEAPONSTATE_HPP__
