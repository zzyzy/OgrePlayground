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
#ifndef __HANDGUN_HPP__
#define __HANDGUN_HPP__

#include "Weapon.hpp"

class HandGun : public WeaponState
{
public:
	explicit HandGun(Ogre::SceneManager* sceneMgr, Ogre::Camera* camera, BulletContext* bulletContext) :
		WeaponState(sceneMgr, camera, bulletContext)
	{
	}

	WeaponState* handleInput(Weapon& weapon, const Input& input) override
	{
		return nullptr;
	}

	void update(Weapon& weapon, const float& deltaTime) override
	{
	}

	void Shoot()
	{
	}

private:
};

#endif // #ifndef __HANDGUN_HPP__
