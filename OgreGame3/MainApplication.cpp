//
// OgreGame3/MainApplication.cpp
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
	mPhysicsContext.Update(evt.timeSinceLastFrame);

	if (!mRTSController.CaptureRenderQueue(evt)) return false;
	if (!GraphicsContext::frameRenderingQueued(evt)) return false;

	return true;
}

bool MainApplication::keyPressed(const OIS::KeyEvent& ke)
{
	return true;
}

bool MainApplication::keyReleased(const OIS::KeyEvent& ke)
{
	if (!mRTSController.CaptureKeyReleased(ke)) return false;
	return true;
}

bool MainApplication::mouseMoved(const OIS::MouseEvent& me)
{
	if (!mRTSController.CaptureMouseMoved(me)) return false;
	if (!GraphicsContext::mouseMoved(me)) return false;
	return true;
}

bool MainApplication::mousePressed(const OIS::MouseEvent& me, OIS::MouseButtonID id)
{
	if (!mRTSController.CaptureMousePressed(me, id, GetTrayMgr())) return false;
	if (!GraphicsContext::mousePressed(me, id)) return false;
	return true;
}

bool MainApplication::mouseReleased(const OIS::MouseEvent& me, OIS::MouseButtonID id)
{
	if (!mRTSController.CaptureMouseReleased(me, id, GetTrayMgr())) return false;
	if (!GraphicsContext::mouseReleased(me, id)) return false;

	return true;
}

void MainApplication::SetupCamera(Ogre::SceneManager* const sceneMgr, Ogre::Camera*& camera)
{
	GraphicsContext::SetupCamera(sceneMgr, camera);
	auto cameraNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
	cameraNode->attachObject(camera);
	cameraNode->translate(0.0f, 90.0f, 100.0f);
	cameraNode->lookAt(Ogre::Vector3(0.0f, 0.0f, 0.0f), Ogre::Node::TS_WORLD);
	mRTSController.AttachCamera(cameraNode);
}

void MainApplication::SetupScene(Ogre::SceneManager* const sceneMgr)
{
	sceneMgr->setAmbientLight(Ogre::ColourValue(1, 1, 1));

	sceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);

	auto* light = sceneMgr->createLight("MainLight");
	light->setPosition(0, 0, 0);

	// Setup ground, walls and ceiling (room)
	{
		// Setup graphics
		Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);

		Ogre::MeshManager::getSingleton().createPlane(
			"plane",
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			plane,
			100, 100, 20, 20,
			true,
			1, 5, 5,
			Ogre::Vector3::UNIT_Z
		);

		// TODO Shadows doesn't seem to be working on planes?
		// Ground
		auto* groundEntity = sceneMgr->createEntity("plane");
		groundEntity->setCastShadows(false);
		auto* groundNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
		groundNode->attachObject(groundEntity);
		groundEntity->setMaterialName("Examples/GrassFloor");

		// Setup rigidbodies
		btTransform transform;
		btVector3 origin;
		btQuaternion rotation;

		// Ground rigidbody
		btBoxShape* groundShape = new btBoxShape(btVector3(50.0f, 5.0f, 50.0f));
		transform.setIdentity();
		transform.setOrigin(btVector3(0, -5, 0));
		mPhysicsContext.CreateRigidBody(0.0f, transform, groundShape, groundNode);
	}
}

void MainApplication::SetupTrayUI(Ogre::SceneManager* const sceneMgr, OgreBites::SdkTrayManager*& trayMgr)
{
	GraphicsContext::SetupTrayUI(sceneMgr, trayMgr);
}
