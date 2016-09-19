//
// OgreGame2/Weapon.hpp
// 
// TODO Add description
// http://gameprogrammingpatterns.com/state.html
//
// Author: Zhen Zhi Lee <leezhenzhi@gmail.com>
//
// Copyright (C) 2016 Zhen Zhi Lee.
//

#pragma once
#ifndef __WEAPON_HPP__
#define __WEAPON_HPP__

#include "WeaponState.hpp"

class Weapon
{
public:
	Weapon() :
		mState(nullptr)
	{
	}

	~Weapon()
	{
		if (mState != nullptr)
		{
			delete mState;
		}
	}

	void SetState(WeaponState* state)
	{
		if (mState != nullptr)
		{
			delete mState;
		}
		mState = state;
	}

	// TODO Temporary. Need a way to get current state.
	const WeaponState* GetState() const
	{
		return mState;
	}

	std::string GetStateName() const
	{
		return mState->name();
	}

	void handleInput(WeaponState::Input input)
	{
		auto* state = mState->handleInput(*this, input);
		if (state != nullptr)
		{
			delete mState;
			mState = state;
		}
	}

	void update(const float& deltaTime)
	{
		mState->update(*this, deltaTime);
	}

private:
	WeaponState* mState;
};

#endif // #ifndef __WEAPON_HPP__
