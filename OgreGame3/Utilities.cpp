//
// OgreGame3/Utilities.cpp
//
// Author: Zhen Zhi Lee <leezhenzhi@gmail.com>
//
// Copyright (C) 2016 Zhen Zhi Lee.
//

#include "Utilities.hpp"

Ogre::Vector3 Convert(const btVector3& vector3)
{
	return Ogre::Vector3(vector3.x(), vector3.y(), vector3.z());
}

btVector3 Convert(const Ogre::Vector3& vector3)
{
	return btVector3(vector3.x, vector3.y, vector3.z);
}

btQuaternion Convert(const Ogre::Quaternion& quaternion)
{
	return btQuaternion(quaternion.x, quaternion.y, quaternion.z, quaternion.w);
}

Ogre::Quaternion Convert(const btQuaternion& quaternion)
{
	return Ogre::Quaternion(quaternion.w(), quaternion.x(), quaternion.y(), quaternion.z());
}

Ogre::Quaternion LookRotation(Ogre::Vector3 forward, Ogre::Vector3 up)
{
	forward = forward.normalisedCopy();
	auto right = (up.crossProduct(forward)).normalisedCopy();
	up = forward.crossProduct(right);
	auto m00 = right.x;
	auto m01 = right.y;
	auto m02 = right.z;
	auto m10 = up.x;
	auto m11 = up.y;
	auto m12 = up.z;
	auto m20 = forward.x;
	auto m21 = forward.y;
	auto m22 = forward.z;

	auto num8 = (m00 + m11) + m22;
	Ogre::Quaternion quaternion;
	if (num8 > 0.0f)
	{
		auto num = static_cast<float>(sqrt(num8 + 1.0f));
		quaternion.w = num * 0.5f;
		num = 0.5f / num;
		quaternion.x = (m12 - m21) * num;
		quaternion.y = (m20 - m02) * num;
		quaternion.z = ((m01 - m10) * num);
		return quaternion;
	}
	if ((m00 >= m11) && (m00 >= m22))
	{
		auto num7 = static_cast<float>(sqrt(((1.0f + m00) - m11) - m22));
		auto num4 = 0.5f / num7;
		quaternion.x = 0.5f * num7;
		quaternion.y = (m01 + m10) * num4;
		quaternion.z = ((m02 + m20) * num4);
		quaternion.w = (m12 - m21) * num4;
		return quaternion;
	}
	if (m11 > m22)
	{
		auto num6 = static_cast<float>(sqrt(((1.0f + m11) - m00) - m22));
		auto num3 = 0.5f / num6;
		quaternion.x = (m10 + m01) * num3;
		quaternion.y = 0.5f * num6;
		quaternion.z = ((m21 + m12) * num3);
		quaternion.w = (m20 - m02) * num3;
		return quaternion;
	}
	auto num5 = static_cast<float>(sqrt(((1.0f + m22) - m00) - m11));
	auto num2 = 0.5f / num5;
	quaternion.x = (m20 + m02) * num2;
	quaternion.y = (m21 + m12) * num2;
	quaternion.z = (0.5f * num5);
	quaternion.w = (m01 - m10) * num2;
	return quaternion;
}
