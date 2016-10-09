//
// OgreGame3/RTSController.cpp
//
// Author: Zhen Zhi Lee <leezhenzhi@gmail.com>
//
// Copyright (C) 2016 Zhen Zhi Lee.
//

#include "RTSController.hpp"

RTSController::RTSController() :
	mCamera(nullptr),
	mSpeed(5.0f),
	mMouseSensitivity(0.25f)
{
}

RTSController::~RTSController()
{
}

void RTSController::AttachCamera(Ogre::SceneNode* camera)
{
	mCamera = camera;
}

bool RTSController::CaptureMouseMoved(const OIS::MouseEvent& me)
{
	if (me.state.X.abs == me.state.width)
	{
		mMove.first = mSpeed;
	}
	else if (me.state.X.abs == 0)
	{
		mMove.first = -mSpeed;
	}
	else
	{
		mMove.first = 0.0f;
	}

	if (me.state.Y.abs == me.state.height)
	{
		mMove.second = mSpeed;
	}
	else if (me.state.Y.abs == 0)
	{
		mMove.second = -mSpeed;
	}
	else
	{
		mMove.second = 0.0f;
	}

	return true;
}

bool RTSController::CaptureMousePressed(const OIS::MouseEvent& me, OIS::MouseButtonID id)
{
	return true;
}

bool RTSController::CaptureMouseReleased(const OIS::MouseEvent& me, OIS::MouseButtonID id)
{
	return true;
}

bool RTSController::CaptureRenderQueue(const Ogre::FrameEvent& fe) const
{
	if (mMove.first != 0.0f || mMove.second != 0.0f)
	{
		Ogre::Vector3 pos = mCamera->getPosition();

		pos.x += mMove.first * mMouseSensitivity;
		pos.z += mMove.second * mMouseSensitivity;

		mCamera->setPosition(pos);
	}
	return true;
}
