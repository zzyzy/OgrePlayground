//
//  OgreBasicTut/CameraController.hpp
//
//  Controls the camera in different styles: free roam, 
//  arcball orbit, and third person
//
//  Author: Zhen Zhi Lee <leezhenzhi@gmail.com>
//
//  Copyright (C) 2016 Zhen Zhi Lee.
//
//  The design of the class is inspired by OgreBites SdkCameraMan
//

#pragma once
#ifndef __CAMERACONTROLLER_HPP__
#define __CAMERACONTROLLER_HPP__

enum class CameraStyle
{
	FREE_ROAM,
	ORBIT,
	THIRD_PERSON
};

class CameraController
{
public:
	CameraController();
	~CameraController();

	// Set to a SceneNode that has an attached Camera
	// Only used for third person style
	void	SetCamera			(Ogre::SceneNode *cam);

	// Set to any SceneNode that will be manipulated 
	// by key presses or mouse events
	void	SetObject			(Ogre::SceneNode *obj);

	// Change the controller style
	void	SetStyle			(const CameraStyle &style);

	// Set the target to orbit around
	// Only for orbit style
	void	SetTarget			(Ogre::SceneNode *target);

	// Keyboard events
	bool	InjectKeyPressed	(const OIS::KeyEvent &ke);
	bool	InjectKeyReleased	(const OIS::KeyEvent &ke);

	// Mouse events
	bool	InjectMouseMoved	(const OIS::MouseEvent &me)							const;
	bool	InjectMousePressed	(const OIS::MouseEvent &me, OIS::MouseButtonID id);
	bool	InjectMouseReleased	(const OIS::MouseEvent &me, OIS::MouseButtonID id);

	// Frame rendering queue event
	bool	InjectRenderQueue	(const Ogre::FrameEvent &fe);

private:
	Ogre::SceneNode		*mCamera;	// Camera node for third person style
	Ogre::SceneNode		*mObject;	// Camera controlled node
	Ogre::SceneNode		*mTarget;	// Target node for orbit style

	CameraStyle			mStyle;

	Ogre::Vector3		mPosition;
	Ogre::Real			mRotate;
	Ogre::Real			mSpeed;
	Ogre::Real			mRollDirection;
	//bool				mRolling;
	bool				mOrbiting;

	Ogre::Real			mMouseMoveSensitivity;
	Ogre::Real			mMouseScrollSensitivity;
};

#endif // #ifndef __CAMERACONTROLLER_HPP__