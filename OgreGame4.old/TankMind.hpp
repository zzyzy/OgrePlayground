/*
 * State machine for tanks
 * 
 * Copyright (C) 2016 Zhen Zhi Lee
 * Written by Zhen Zhi Lee (leezhenzhi@gmail.com)
 * 
 * Manages the state of the tank.
 */

#pragma once
#ifndef __TANK_MIND_HPP__
#define __TANK_MIND_HPP__

#include "ITankState.hpp"

class TankMind
{
public:
    TankMind() :
        mState(nullptr)
    {
    }

private:
    ITankState* mState;
};

#endif // __TANK_MIND_HPP__
