//
// OgreGame3/Utilities.hpp
// 
// TODO Add description
//
// Author: Zhen Zhi Lee <leezhenzhi@gmail.com>
//
// Copyright (C) 2016 Zhen Zhi Lee.
//

#pragma once
#ifndef __UTILITIES_HPP__
#define __UTILITIES_HPP__

Ogre::Vector3 Convert(const btVector3& vector3);

btVector3 Convert(const Ogre::Vector3& vector3);

btQuaternion Convert(const Ogre::Quaternion& quaternion);

Ogre::Quaternion Convert(const btQuaternion& quaternion);

Ogre::Quaternion LookRotation(Ogre::Vector3 forward, Ogre::Vector3 up);

#endif // __UTILITIES_HPP__
