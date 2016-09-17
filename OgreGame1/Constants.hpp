//
//  OgreBasicTut/Constants.hpp
//
//  Contains all constants used by the application
//
//  Author: Zhen Zhi Lee <leezhenzhi@gmail.com>
//
//  Copyright (C) 2016 Zhen Zhi Lee.
//

#pragma once
#ifndef __CONSTANTS_HPP__
#define __CONSTANTS_HPP__

// k for constant
namespace k
{
	static const Ogre::String		WINDOW_TITLE		= "Assessment 1";
	static const Ogre::String		CAMERA_NAME			= "MainCam";
	static const Ogre::String		RESOURCES_D_FILE	= "resources_d.cfg";
	static const Ogre::String		PLUGINS_D_FILE		= "plugins_d.cfg";
	static const Ogre::String		RESOURCES_FILE		= "resources.cfg";
	static const Ogre::String		PLUGINS_FILE		= "plugins.cfg";
	static const size_t				NUM_MIDMAP			= 5;

	static const Ogre::Vector3		CAMERA_START_POS	(0, 50, 80);
	static const Ogre::Vector3		CAMERA_START_LOOKAT	(0, 0, -300);
	static const Ogre::Real			CAMERA_NEAR_DIST	(5);

	static const Ogre::ColourValue	VIEWPORT_BG_COLOUR	(0.5, 0.5, 0.5);

	static const Ogre::ColourValue	AMBIENT_COLOUR		(0.5, 0.5, 0.5);

	// Entities and nodes
	static const Ogre::String		OGRE_ENTITY			= "ogrehead.mesh";
	static const Ogre::String		ROBOT_ENTITY		= "robot.mesh";

	static const Ogre::String		CAMERA_NODE			= "CameraNode";
	static const Ogre::String		ROBOT_CAMERA_NODE	= "RobotCameraNode";
	static const Ogre::String		ROBOT_NODE			= "RobotNode";

	// Lights
	static const Ogre::String		MAIN_LIGHT			= "MainLight";
	static const Ogre::Vector3		MAIN_LIGHT_POS		(20, 80, 50);
}

#endif // #ifndef __CONSTANTS_HPP__
