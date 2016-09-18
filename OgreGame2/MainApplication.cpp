//
// OgreGame2/MainApplication.cpp
// 
// Author: Zhen Zhi Lee <leezhenzhi@gmail.com>
//
// Copyright (C) 2016 Zhen Zhi Lee.
//

#include "MainApplication.hpp"
#include "BallShooter.hpp"
#include "HandGun.hpp"

MainApplication::MainApplication()
{
	mBulletContext.Setup();
}

MainApplication::~MainApplication()
{
}

bool MainApplication::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	if (!OgreContext::frameRenderingQueued(evt)) return false;
	if (!mFPSController.CaptureRenderQueue(evt)) return false;
	mBulletContext.Update(evt.timeSinceLastFrame);
	mWeapon.update(evt.timeSinceLastFrame);
	return true;
}

bool MainApplication::keyPressed(const OIS::KeyEvent& ke)
{
	if (!mFPSController.CaptureKeyPressed(ke)) return false;

	switch (ke.key)
	{
	case OIS::KC_1:
		mWeapon.SetState(new BallShooter(mSceneMgr, mCamera, &mBulletContext));
		break;
	case OIS::KC_2:
		mWeapon.SetState(new HandGun(mSceneMgr, mCamera, &mBulletContext));
		break;
	default:
		break;
	}

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

	if (id == OIS::MB_Left)
	{
		mWeapon.handleInput(WeaponState::Input::MOUSEDOWN);
	}

	return true;
}

bool MainApplication::mouseReleased(const OIS::MouseEvent& me, OIS::MouseButtonID id)
{
	if (!mFPSController.CaptureMouseReleased(me, id)) return false;

	if (id == OIS::MB_Left)
	{
		mWeapon.handleInput(WeaponState::Input::MOUSEUP);
	}

	return true;
}

void MainApplication::setupScene(Ogre::SceneManager* const sceneMgr)
{
	sceneMgr->setAmbientLight(Ogre::ColourValue(1, 1, 1));

	auto* light = sceneMgr->createLight("MainLight");
	light->setPosition(0, 0, 0);

	Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);
	/*
	Ogre::MeshManager::getSingleton().createPlane(
	    "plane",
	    Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
	    plane,
	    500, 500, 20, 20,
	    true,
	    1, 5, 5,
	    Ogre::Vector3::UNIT_Z
	);
	*/
	Ogre::MeshManager::getSingleton().createPlane("ground", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane,
	                                              100, 100, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Z);

	auto* groundEntity = sceneMgr->createEntity("ground");
	sceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(groundEntity);
	groundEntity->setMaterialName("Examples/Rockwall");

	// Create the ground object shape
	btBoxShape* groundShape = new btBoxShape(btVector3(50.0f, 5.0f, 50.0f));

	// Set the object's transformation
	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(0, -5, 0));

	Ogre::Entity* cube = mSceneMgr->createEntity("Ground", "cube.mesh");
	Ogre::SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	node->attachObject(cube);
	node->scale(1.0f, 0.1f, 1.0f);
	node->translate(0.0f, -5.0f, 0.0f);

	// Create the rigid body
	mBulletContext.CreateRigidBody(0.0f, groundTransform, groundShape, node);
	node->setVisible(false);

	auto fpsPlayer = sceneMgr->getRootSceneNode()->createChildSceneNode("FPSPlayer");
	auto cameraNode = fpsPlayer->createChildSceneNode("CameraNode");
	cameraNode->attachObject(mCamera);
	mCamera->setPosition(0.0f, 0.0f, 0.0f);
	cameraNode->setPosition(0.0f, 10.0f, 0.0f);
	mFPSController.Attach(cameraNode, fpsPlayer);

	mWeapon.SetState(new BallShooter(mSceneMgr, mCamera, &mBulletContext));
}
