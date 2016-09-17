//
// OgreGame2/FPSController.hpp
// 
// This is a stripdown version of camera controller from OgreGame1.
// Keyboard and mouse controller for FPS. FPSController requires the head and
// the body of the controlled object to function properly. The head should have
// a camera attached to it, and the head should be attached to the body. When
// changing the subject's facing using the mouse, changing yaw settings will
// turn the body, whereas changing pitch settings will lift the head, without
// lifting the whole body. Common WASD keys will move the subject just like
// what a normal FPS controller should do.
//
// Author: Zhen Zhi Lee <leezhenzhi@gmail.com>
//
// Copyright (C) 2016 Zhen Zhi Lee.
//

#pragma once
#ifndef __FPSCONTROLLER_HPP__
#define __FPSCONTROLLER_HPP__

class FPSController
{
public:
    FPSController();
    ~FPSController();

    void Attach(Ogre::SceneNode* const head, Ogre::SceneNode* const body);
    void AttachHead(Ogre::SceneNode* const head);
    void AttachBody(Ogre::SceneNode* const body);

    // Keyboard events
    bool CaptureKeyPressed(const OIS::KeyEvent& ke);
    bool CaptureKeyReleased(const OIS::KeyEvent& ke);

    // Mouse events
    bool CaptureMouseMoved(const OIS::MouseEvent& me) const;
    bool CaptureMousePressed(const OIS::MouseEvent& me, OIS::MouseButtonID id);
    bool CaptureMouseReleased(const OIS::MouseEvent& me, OIS::MouseButtonID id);

    // Frame rendering queue event
    bool CaptureRenderQueue(const Ogre::FrameEvent& fe) const;

private:
    Ogre::SceneNode* mHead;
    Ogre::SceneNode* mBody;
    Ogre::Vector3 mPosition;
    Ogre::Real mSpeed;
    Ogre::Real mMouseSensitivity;
};

#endif // #ifndef __FPSCONTROLLER_HPP__
