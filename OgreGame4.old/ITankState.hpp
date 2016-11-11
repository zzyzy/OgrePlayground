/*
 * Interface for tank states
 * 
 * Copyright (C) 2016 Zhen Zhi Lee
 * Written by Zhen Zhi Lee (leezhenzhi@gmail.com)
 * 
 * All possible states for the tank must implement
 * this interface for the state machine pattern 
 * to work.
 */

#pragma once
#ifndef __I_TANK_STATE_HPP__
#define __I_TANK_STATE_HPP__

class TankMind;

class ITankState
{
public:
    enum class Input
    {
    };

    virtual ~ITankState()
    {
    }

    virtual void HandleInput(TankMind& tank, Input input) = 0;
    virtual void Update(TankMind& tank) = 0;
};

#endif // __I_TANK_STATE_HPP__
