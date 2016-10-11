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
	mBaseSpeed(10.0f),
	mSpeed(mBaseSpeed),
	mSensitivity(0.05f),
	mIsRotating(false)
{
}

RTSController::~RTSController()
{
}

void RTSController::AttachCamera(Ogre::SceneNode* camera)
{
	mCamera = camera;
	mDefaultPos = mCamera->getPosition();
	mDefaultOrientation = mCamera->getOrientation();
	mPosition = Ogre::Vector3::ZERO;
	mSpeed = mBaseSpeed * mSensitivity;
}

bool RTSController::CaptureKeyReleased(const OIS::KeyEvent& ke) const
{
	switch (ke.key)
	{
	case OIS::KC_SPACE:
		mCamera->setPosition(mDefaultPos);
		mCamera->setOrientation(mDefaultOrientation);
		break;
	default:
		break;
	}

	return true;
}

bool RTSController::CaptureMouseMoved(const OIS::MouseEvent& me)
{
	if (mIsRotating)
	{
		mCamera->yaw(Ogre::Degree(-me.state.X.rel * 0.25f * mSpeed), Ogre::Node::TS_WORLD);
		mCamera->pitch(Ogre::Degree(-me.state.Y.rel * 0.25f * mSpeed), Ogre::Node::TS_LOCAL);
		return true;
	}

	if (me.state.X.abs == me.state.width)
	{
		mPosition.x = mSpeed;
	}
	else if (me.state.X.abs == 0)
	{
		mPosition.x = -mSpeed;
	}
	else
	{
		mPosition.x = 0.0f;
	}

	if (me.state.Y.abs == me.state.height)
	{
		mPosition.y = -mSpeed;
	}
	else if (me.state.Y.abs == 0)
	{
		mPosition.y = mSpeed;
	}
	else
	{
		mPosition.y = 0.0f;
	}

	if (me.state.Z.rel != 0)
	{
		auto position = mCamera->getPosition();
		position.x = 0.0f;
		position.y = 0.0f;
		position.z = -me.state.Z.rel * 0.05f * mSpeed;
		mCamera->translate(position, Ogre::Node::TS_LOCAL);
	}

	return true;
}

bool RTSController::CaptureMousePressed(const OIS::MouseEvent& me, const OIS::MouseButtonID& id, OgreBites::SdkTrayManager* trayMgr)
{
	if (id == OIS::MB_Middle)
	{
		mIsRotating = true;
		trayMgr->hideCursor();
	}

	return true;
}

bool RTSController::CaptureMouseReleased(const OIS::MouseEvent& me, const OIS::MouseButtonID& id, OgreBites::SdkTrayManager* trayMgr)
{
	if (id == OIS::MB_Middle)
	{
		mIsRotating = false;
		trayMgr->showCursor();
	}

	return true;
}

bool RTSController::CaptureRenderQueue(const Ogre::FrameEvent& fe)
{
	mPosition.z = 0.0f;

	if (mPosition != Ogre::Vector3::ZERO)
	{
		mCamera->translate(mPosition, Ogre::Node::TS_LOCAL);
	}

	return true;
}
