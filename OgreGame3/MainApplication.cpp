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
	mPhysicsContext.Setup();
	mGraphicsContext.Setup(this, this, this, this, this);
}

MainApplication::~MainApplication()
{
}

bool MainApplication::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	mPhysicsContext.Update(evt.timeSinceLastFrame);
	if (!mRTSController.CaptureRenderQueue(evt)) return false;
	if (!mGraphicsContext.CaptureRenderQueue(evt)) return false;
	return true;
}

bool MainApplication::keyPressed(const OIS::KeyEvent& ke)
{
	if (!mObjectSelector.CaptureKeyPressed(ke)) return false;
	return true;
}

bool MainApplication::keyReleased(const OIS::KeyEvent& ke)
{
	if (!mRTSController.CaptureKeyReleased(ke)) return false;
	if (!mObjectSelector.CaptureKeyReleased(ke)) return false;
	return true;
}

bool MainApplication::mouseMoved(const OIS::MouseEvent& me)
{
	if (!mRTSController.CaptureMouseMoved(me)) return false;
	if (!mObjectSelector.CaptureMouseMoved(me)) return false;
	if (!mGraphicsContext.CaptureMouseMoved(me)) return false;
	return true;
}

bool MainApplication::mousePressed(const OIS::MouseEvent& me, OIS::MouseButtonID id)
{
	if (!mRTSController.CaptureMousePressed(me, id, mGraphicsContext.GetTrayMgr())) return false;
	if (!mObjectSelector.CaptureMousePressed(me, id)) return false;
	if (!mGraphicsContext.CaptureMousePressed(me, id)) return false;
	return true;
}

bool MainApplication::mouseReleased(const OIS::MouseEvent& me, OIS::MouseButtonID id)
{
	if (!mRTSController.CaptureMouseReleased(me, id, mGraphicsContext.GetTrayMgr())) return false;
	if (!mObjectSelector.CaptureMouseReleased(me, id)) return false;
	if (!mGraphicsContext.CaptureMouseReleased(me, id)) return false;
	return true;
}

void MainApplication::SetupCamera(Ogre::SceneManager* const sceneMgr, Ogre::Camera* camera)
{
	auto cameraNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
	cameraNode->attachObject(camera);
	cameraNode->translate(0.0f, 100.0f, 0.0f);
	cameraNode->lookAt(Ogre::Vector3(0.0f, 0.0f, 0.0f), Ogre::Node::TS_WORLD);
	mRTSController.AttachCamera(cameraNode);
	mObjectSelector.Setup(sceneMgr, camera);
}

void MainApplication::SetupViewport(Ogre::RenderWindow* const window, Ogre::Camera* camera)
{
}

void MainApplication::SetupTrayUI(Ogre::SceneManager* const sceneMgr, OgreBites::SdkTrayManager* trayMgr)
{
}

void MainApplication::SetupScene(Ogre::SceneManager* const sceneMgr)
{
	sceneMgr->setAmbientLight(Ogre::ColourValue(1, 1, 1));
	sceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);

	auto* light = sceneMgr->createLight("MainLight");
	light->setPosition(0, 0, 0);
	light->setQueryFlags(0);

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
		groundEntity->setQueryFlags(ENVIRONMENT_MASK);
		groundEntity->setMaterialName("Examples/GrassFloor");
		auto* groundNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
		groundNode->attachObject(groundEntity);

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
