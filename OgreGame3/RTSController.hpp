//
// OgreGame3/RTSController.hpp
// 
// TODO Add description
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

	void AttachCamera(Ogre::SceneNode* camera);

	// Mouse events
	bool CaptureMouseMoved(const OIS::MouseEvent& me);
	bool CaptureMousePressed(const OIS::MouseEvent& me, OIS::MouseButtonID id);
	bool CaptureMouseReleased(const OIS::MouseEvent& me, OIS::MouseButtonID id);

	// Frame rendering queue event
	bool CaptureRenderQueue(const Ogre::FrameEvent& fe);

private:
	Ogre::SceneNode* mCamera;
	Ogre::Real mBaseSpeed;
	Ogre::Real mSpeed;
	Ogre::Real mSensitivity;
	bool mRMBHold;
	std::pair<float, float> mMove;
};

#endif // __RTSCONTROLLER_HPP__
