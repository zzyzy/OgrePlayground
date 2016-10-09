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
	mBaseSpeed(50.0f),
	mSpeed(mBaseSpeed),
	mSensitivity(4.0f),
	mRMBHold(false)
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
	if (mRMBHold)
	{
		mCamera->yaw(Ogre::Degree(-me.state.X.rel * 0.25f * mSpeed), Ogre::Node::TS_WORLD);
		mCamera->pitch(Ogre::Degree(-me.state.Y.rel * 0.25f * mSpeed), Ogre::Node::TS_LOCAL);
		return true;
	}

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

	if (me.state.Z.rel != 0)
	{
		auto pos = mCamera->getPosition();
		pos.y += -me.state.Z.rel * 0.25f * mSpeed;
		mCamera->setPosition(pos);
	}

	return true;
}

bool RTSController::CaptureMousePressed(const OIS::MouseEvent& me, OIS::MouseButtonID id)
{
	if (id == OIS::MB_Right)
	{
		mRMBHold = true;
	}

	return true;
}

bool RTSController::CaptureMouseReleased(const OIS::MouseEvent& me, OIS::MouseButtonID id)
{
	if (id == OIS::MB_Right)
	{
		mRMBHold = false;
	}

	return true;
}

bool RTSController::CaptureRenderQueue(const Ogre::FrameEvent& fe)
{
	mSpeed = mBaseSpeed * mSensitivity * fe.timeSinceLastFrame;

	if (mMove.first != 0.0f || mMove.second != 0.0f)
	{
		auto pos = mCamera->getPosition();

		pos.x += mMove.first;
		pos.z += mMove.second;

		mCamera->setPosition(pos);
	}

	return true;
}
