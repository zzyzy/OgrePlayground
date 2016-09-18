//
// OgreGame2/FPSController.cpp
//
// Author: Zhen Zhi Lee <leezhenzhi@gmail.com>
//
// Copyright (C) 2016 Zhen Zhi Lee.
//

#include "FPSController.hpp"

FPSController::FPSController() :
	mHead(nullptr),
	mBody(nullptr),
	mPosition(0, 0, 0),
	mSpeed(200), // Movement speed
	mMouseSensitivity(0.25) // Change this for mouse sensitivity
{
}

FPSController::~FPSController()
{
	mHead = nullptr;
	mBody = nullptr;
}

void FPSController::Attach(Ogre::SceneNode* const head, Ogre::SceneNode* const body)
{
	mHead = head;
	mBody = body;
}

void FPSController::AttachHead(Ogre::SceneNode* const head)
{
	mHead = head;
}

void FPSController::AttachBody(Ogre::SceneNode* const body)
{
	mBody = body;
}

bool FPSController::CaptureKeyPressed(const OIS::KeyEvent& ke)
{
	// Split into two to allow the user to move 
	// forward/backward and strafe left/right at 
	// the same time

	if (ke.key == OIS::KC_W)
	{
		mPosition.z += -mSpeed; // Move into the screen (forward)
	}
	else if (ke.key == OIS::KC_S)
	{
		mPosition.z += mSpeed;
	}

	if (ke.key == OIS::KC_A)
	{
		mPosition.x += -mSpeed; // Move to the left of the screen (left)
	}
	else if (ke.key == OIS::KC_D)
	{
		mPosition.x += mSpeed;
	}

	return true;
}

bool FPSController::CaptureKeyReleased(const OIS::KeyEvent& ke)
{
	if (ke.key == OIS::KC_W)
	{
		mPosition.z += mSpeed;
	}
	else if (ke.key == OIS::KC_S)
	{
		mPosition.z += -mSpeed;
	}

	if (ke.key == OIS::KC_A)
	{
		mPosition.x += mSpeed;
	}
	else if (ke.key == OIS::KC_D)
	{
		mPosition.x += -mSpeed;
	}

	return true;
}

bool FPSController::CaptureMouseMoved(const OIS::MouseEvent& me) const
{
	// The body turns completely when moving the mouse along the X axis (of the screen)
	mBody->yaw(Ogre::Degree(-me.state.X.rel * mMouseSensitivity), Ogre::Node::TS_WORLD);

	// Only the head will be affected by pitch when moving the mouse along the Y axis
	mHead->pitch(Ogre::Degree(-me.state.Y.rel * mMouseSensitivity), Ogre::Node::TS_LOCAL);

	// From http://www.ogre3d.org/tikiwiki/Creating+a+simple+first-person+camera+system
	// Angle of rotation around the X-axis.
	auto pitchAngle = (2 * Ogre::Degree(Ogre::Math::ACos(mHead->getOrientation().w)).valueDegrees());

	// Just to determine the sign of the angle we pick up above, the
	// value itself does not interest us.
	auto pitchAngleSign = mHead->getOrientation().x;

	// Limit the pitch between -90 degress and +90 degrees, Quake3-style.
	if (pitchAngle > 90.0f)
	{
		if (pitchAngleSign > 0)
		// Set orientation to 90 degrees on X-axis.
			mHead->setOrientation(Ogre::Quaternion(Ogre::Math::Sqrt(0.5f),
			                                       Ogre::Math::Sqrt(0.5f), 0, 0));
		else if (pitchAngleSign < 0)
		// Sets orientation to -90 degrees on X-axis.
			mHead->setOrientation(Ogre::Quaternion(Ogre::Math::Sqrt(0.5f),
			                                       -Ogre::Math::Sqrt(0.5f), 0, 0));
	}

	return true;
}

bool FPSController::CaptureMousePressed(const OIS::MouseEvent& me, OIS::MouseButtonID id)
{
	return true;
}

bool FPSController::CaptureMouseReleased(const OIS::MouseEvent& me, OIS::MouseButtonID id)
{
	return true;
}

bool FPSController::CaptureRenderQueue(const Ogre::FrameEvent& fe) const
{
	// Smooth movement
	mBody->translate(mPosition * fe.timeSinceLastFrame, Ogre::Node::TS_LOCAL);
	return true;
}
