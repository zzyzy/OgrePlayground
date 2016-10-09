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
	if (!OgreContext::frameRenderingQueued(evt)) return false;

	mBulletContext.Update(evt.timeSinceLastFrame);

	return true;
}

bool MainApplication::keyPressed(const OIS::KeyEvent& ke)
{
	return true;
}

bool MainApplication::keyReleased(const OIS::KeyEvent& ke)
{
	return true;
}

bool MainApplication::mouseMoved(const OIS::MouseEvent& me)
{
	if (!OgreContext::mouseMoved(me)) return false;
	return true;
}

bool MainApplication::mousePressed(const OIS::MouseEvent& me, OIS::MouseButtonID id)
{
	if (!OgreContext::mousePressed(me, id)) return false;
	return true;
}

bool MainApplication::mouseReleased(const OIS::MouseEvent& me, OIS::MouseButtonID id)
{
	if (!OgreContext::mouseReleased(me, id)) return false;
	return true;
}

void MainApplication::setupCamera(Ogre::SceneManager* const sceneMgr, Ogre::Camera*& camera) const
{
	OgreContext::setupCamera(sceneMgr, camera);
	camera->setPosition(0.0f, 80.0f, 100.0f);
	camera->lookAt(0.0f, 0.0f, 0.0f);
}

void MainApplication::setupScene(Ogre::SceneManager* const sceneMgr)
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
		mBulletContext.CreateRigidBody(0.0f, transform, groundShape, groundNode);
	}
}

void MainApplication::setupTrayUI(Ogre::SceneManager* const sceneMgr, OgreBites::SdkTrayManager*& trayMgr)
{
	OgreContext::setupTrayUI(sceneMgr, trayMgr);
}
