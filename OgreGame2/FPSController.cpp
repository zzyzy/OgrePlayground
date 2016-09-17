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
