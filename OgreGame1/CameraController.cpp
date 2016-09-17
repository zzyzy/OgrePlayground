//
//  OgreBasicTut/CameraController.cpp
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

#include "CameraController.hpp"

CameraController::CameraController() : 
	mCamera(nullptr),
	mObject(nullptr),
	mTarget(nullptr),
	mStyle(CameraStyle::FREE_ROAM),
	mPosition(Ogre::Vector3::ZERO),
	mRotate(0),
	mSpeed(200),
	mRollDirection(0),
	//mRolling(false),
	mOrbiting(false),
	mMouseMoveSensitivity(0.25),
	mMouseScrollSensitivity(0.0008)
{
	
}

CameraController::~CameraController()
{

}

void CameraController::SetCamera(Ogre::SceneNode *cam)
{
	mCamera = cam;
}

void CameraController::SetObject(Ogre::SceneNode *obj)
{
	mObject = obj;
}

void CameraController::SetStyle(const CameraStyle &style)
{
	mStyle = style;
}

void CameraController::SetTarget(Ogre::SceneNode *target)
{
	mTarget = target;
}

bool CameraController::InjectKeyPressed(const OIS::KeyEvent &ke)
{
	switch (mStyle)
	{
	case CameraStyle::FREE_ROAM:
	{
		switch (ke.key)
		{
		case OIS::KC_W:
			mPosition.z = -mSpeed;
			break;
		case OIS::KC_A:
			mPosition.x = -mSpeed;
			break;
		case OIS::KC_S:
			mPosition.z = mSpeed;
			break;
		case OIS::KC_D:
			mPosition.x = mSpeed;
			break;
		case OIS::KC_PGUP:
			mPosition.y = mSpeed;
			break;
		case OIS::KC_PGDOWN:
			mPosition.y = -mSpeed;
			break;
		case OIS::KC_Q:
			mRollDirection = mSpeed;
			//mRolling = true;
			break;
		case OIS::KC_E:
			mRollDirection = -mSpeed;
			//mRolling = true;
			break;
		default:
			break;
		}
		break;
	}
	case CameraStyle::THIRD_PERSON:
	{
		switch (ke.key)
		{
		case OIS::KC_W:
			mPosition.x = mSpeed;
			break;
		case OIS::KC_A:
			mPosition.z = -mSpeed;
			break;
		case OIS::KC_S:
			mPosition.x = -mSpeed;
			break;
		case OIS::KC_D:
			mPosition.z = mSpeed;
			break;
		case OIS::KC_LEFT:
			mRotate = mSpeed;
			break;
		case OIS::KC_RIGHT:
			mRotate = -mSpeed;
			break;
		default:
			break;
		}
		break;
	}
	default:
		break;
	}

	return true;
}

bool CameraController::InjectKeyReleased(const OIS::KeyEvent &ke)
{
	switch (mStyle)
	{
	case CameraStyle::FREE_ROAM:
	{
		switch (ke.key)
		{
		case OIS::KC_W:
			mPosition.z = 0;
			break;
		case OIS::KC_A:
			mPosition.x = 0;
			break;
		case OIS::KC_S:
			mPosition.z = 0;
			break;
		case OIS::KC_D:
			mPosition.x = 0;
			break;
		case OIS::KC_PGUP:
			mPosition.y = 0;
			break;
		case OIS::KC_PGDOWN:
			mPosition.y = 0;
			break;
		case OIS::KC_Q:
			mRollDirection = 0;
			//mRolling = false;
			break;
		case OIS::KC_E:
			mRollDirection = 0;
			//mRolling = false;
			break;
		default:
			break;
		}
		break;
	}
	case CameraStyle::THIRD_PERSON:
	{
		switch (ke.key)
		{
		case OIS::KC_W:
			mPosition.x = 0;
			break;
		case OIS::KC_A:
			mPosition.z = 0;
			break;
		case OIS::KC_S:
			mPosition.x = 0;
			break;
		case OIS::KC_D:
			mPosition.z = 0;
			break;
		case OIS::KC_LEFT:
			mRotate = 0;
			break;
		case OIS::KC_RIGHT:
			mRotate = 0;
			break;
		default:
			break;
		}
		break;
	}
	default:
		break;
	}

	return true;
}

bool CameraController::InjectMouseMoved(const OIS::MouseEvent &me) const
{
	switch (mStyle)
	{
	case CameraStyle::FREE_ROAM:
	{
		// yaw means rotate around the Y axis, which means left and right, 
		// and from screen space the mouse is moving along the X axis
		// pitch means look up or down, which also means rotate around the Z axis
		// from screen space, the mouse is moving along the Y axis
		// TODO Why TS_WORLD?
		mObject->yaw(Ogre::Degree(-me.state.X.rel * mMouseMoveSensitivity), Ogre::Node::TS_WORLD);
		mObject->pitch(Ogre::Degree(-me.state.Y.rel * mMouseMoveSensitivity), Ogre::Node::TS_LOCAL);
		break;
	}
	case CameraStyle::ORBIT:
	{
		// Get the distance between the camera controlled object
		auto dist = (mObject->getPosition() - mTarget->_getDerivedPosition()).length();

		if (mOrbiting) {
			// Set to the position of the target object
			mObject->setPosition(mTarget->_getDerivedPosition());

			// Apply the same yaw and pitch as free roaming
			mObject->yaw(Ogre::Degree(-me.state.X.rel * mMouseMoveSensitivity), Ogre::Node::TS_WORLD);
			mObject->pitch(Ogre::Degree(-me.state.Y.rel * mMouseMoveSensitivity), Ogre::Node::TS_LOCAL);

			// Translate away from the target object ONLY ALONG THE Z AXIS,
			// keeping the same X and Y axis that was manipulated by pitch and yaw
			mObject->translate(Ogre::Vector3(0, 0, dist), Ogre::Node::TS_LOCAL);
		}

		// Is it mouse scrolling?
		if (me.state.Z.rel != 0)
		{
			// Zoom in by translating closer to the object only along the Z axis
			mObject->translate(Ogre::Vector3(0, 0, -me.state.Z.rel * mMouseScrollSensitivity * dist), Ogre::Node::TS_LOCAL);
		}
		break;
	}
	case CameraStyle::THIRD_PERSON:
	{
		// Same thing as before
		// Note the difference between mObject and mCamera as opposed to before
		mObject->yaw(Ogre::Degree(-me.state.X.rel * mMouseMoveSensitivity), Ogre::Node::TS_WORLD);
		mCamera->pitch(Ogre::Degree(-me.state.Y.rel * mMouseMoveSensitivity), Ogre::Node::TS_LOCAL);

		// Same as before
		if (me.state.Z.rel != 0)
		{
			auto dist = (mCamera->getPosition() - mObject->_getDerivedPosition()).length();
			mCamera->translate(Ogre::Vector3(0, 0, -me.state.Z.rel * mMouseScrollSensitivity * dist), Ogre::Node::TS_LOCAL);
		}
		break;
	}
	default:
		break;
	}

	return true;
}

bool CameraController::InjectMousePressed(const OIS::MouseEvent &me, OIS::MouseButtonID id)
{
	// Self explanatory
	if (mStyle == CameraStyle::ORBIT)
	{
		mOrbiting = true;
	}

	return true;
}

bool CameraController::InjectMouseReleased(const OIS::MouseEvent &me, OIS::MouseButtonID id)
{
	if (mStyle == CameraStyle::ORBIT)
	{
		mOrbiting = false;
	}

	return true;
}

bool CameraController::InjectRenderQueue(const Ogre::FrameEvent &fe)
{
	switch (mStyle)
	{
	case CameraStyle::FREE_ROAM:
	{
		// Apply smooth rolling and moving
		mObject->roll(Ogre::Degree(mRollDirection) * fe.timeSinceLastFrame);
		mObject->translate(mPosition * fe.timeSinceLastFrame, Ogre::Node::TS_LOCAL);
		break;
	}
	case CameraStyle::THIRD_PERSON:
	{
		// Apply smooth rotation and moving
		mObject->rotate(Ogre::Vector3::UNIT_Y, Ogre::Degree(mRotate) * fe.timeSinceLastFrame, Ogre::Node::TS_LOCAL);
		mObject->translate(mPosition * fe.timeSinceLastFrame, Ogre::Node::TS_LOCAL);
		break;
	}
	default:
		break;
	}
	
	return true;
}