//
// OgreGame3/RTSController.hpp
// 
// TODO Add description
//
// Basic usage:
// 1. Add it to a class as a member
// 2. Attach camera on initialization
// 3. Setup the appropriate event listeners
//
// Author: Zhen Zhi Lee <leezhenzhi@gmail.com>
//
// Copyright (C) 2016 Zhen Zhi Lee.
//

#pragma once
#ifndef __RTSCONTROLLER_HPP__
#define __RTSCONTROLLER_HPP__

class RTSController
{
public:
	RTSController();
	~RTSController();

	// Attach scene node that has a camera in it
	void AttachCamera(Ogre::SceneNode* camera);

	// Keyboard events
	bool CaptureKeyReleased(const OIS::KeyEvent& ke) const;

	// Mouse events
	bool CaptureMouseMoved(const OIS::MouseEvent& me);
	bool CaptureMousePressed(const OIS::MouseEvent& me, const OIS::MouseButtonID& id, OgreBites::SdkTrayManager* trayMgr);
	bool CaptureMouseReleased(const OIS::MouseEvent& me, const OIS::MouseButtonID& id, OgreBites::SdkTrayManager* trayMgr);

	// Frame rendering queue event
	bool CaptureRenderQueue(const Ogre::FrameEvent& fe);

private:
	Ogre::SceneNode* mCamera;				// Scene node with camera attached
	Ogre::Vector3 mDefaultPos;				// Default camera position
	Ogre::Quaternion mDefaultOrientation;	// Default camera orientation
	Ogre::Vector3 mPosition;				// Camera (scene node) position
	const Ogre::Real mBaseSpeed;			// Base speed that is not modified
	Ogre::Real mSpeed;						// Speed that will be multiplied with sensitivity
	Ogre::Real mSensitivity;				// ^
	bool mIsRotating;						// If camera is currently rotating
};

#endif // __RTSCONTROLLER_HPP__
