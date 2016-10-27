/*
 * Collision masks for this game
 *
 * Copyright (C) 2016 Zhen Zhi Lee
 * Written by Zhen Zhi Lee (leezhenzhi@gmail.com)
 * 
 * http://www.bulletphysics.org/mediawiki-1.5.8/index.php/Collision_Filtering
 */

#pragma once
#ifndef __COLLISION_MASKS_HPP__
#define __COLLISION_MASKS_HPP__

#define BIT(x) (1<<(x))

enum CollisionTypes
{
    COL_NOTHING = 0, //<Collide with nothing
    COL_TANK = BIT(0), //<Collide with ships
    COL_GROUND = BIT(1), //<Collide with walls
    COL_POWERUP = BIT(2), //<Collide with powerups
    COL_EXPLOSION = BIT(3),
    COL_PROJECTILE = BIT(4),
    COL_ENVIRONMENT_OBJECT = BIT(5)
};

#endif // __COLLISION_MASKS_HPP__
