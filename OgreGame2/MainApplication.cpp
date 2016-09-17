//
// OgreGame2/MainApplication.cpp
// 
// Author: Zhen Zhi Lee <leezhenzhi@gmail.com>
//
// Copyright (C) 2016 Zhen Zhi Lee.
//

#include "MainApplication.hpp"

MainApplication::MainApplication()
{
}

MainApplication::~MainApplication()
{
}

bool MainApplication::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    if (!OgreContext::frameRenderingQueued(evt)) return false;
    if (!mFPSController.CaptureRenderQueue(evt)) return false;
    return true;
}

bool MainApplication::keyPressed(const OIS::KeyEvent& ke)
{
    if (!mFPSController.CaptureKeyPressed(ke)) return false;
    return true;
}

bool MainApplication::keyReleased(const OIS::KeyEvent& ke)
{
    if (!mFPSController.CaptureKeyReleased(ke)) return false;
    return true;
}

bool MainApplication::mouseMoved(const OIS::MouseEvent& me)
{
    if (!mFPSController.CaptureMouseMoved(me)) return false;
    return true;
}

bool MainApplication::mousePressed(const OIS::MouseEvent& me, OIS::MouseButtonID id)
{
    if (!mFPSController.CaptureMousePressed(me, id)) return false;
    return true;
}

bool MainApplication::mouseReleased(const OIS::MouseEvent& me, OIS::MouseButtonID id)
{
    if (!mFPSController.CaptureMouseReleased(me, id)) return false;
    return true;
}

void MainApplication::setupScene(Ogre::SceneManager* const sceneMgr)
{
    sceneMgr->setAmbientLight(Ogre::ColourValue(1, 1, 1));

    auto* light = sceneMgr->createLight("MainLight");
    light->setPosition(0, 0, 0);

    Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);
    Ogre::MeshManager::getSingleton().createPlane(
        "plane",
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        plane,
        500, 500, 20, 20,
        true,
        1, 5, 5,
        Ogre::Vector3::UNIT_Z
    );

    auto* groundEntity = sceneMgr->createEntity("plane");
    sceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(groundEntity);
    groundEntity->setMaterialName("Examples/Rockwall");

    auto fpsPlayer = sceneMgr->getRootSceneNode()->createChildSceneNode("FPSPlayer");
    auto cameraNode = fpsPlayer->createChildSceneNode("CameraNode");
    cameraNode->attachObject(mCamera);
    mFPSController.Attach(cameraNode, fpsPlayer);
}
