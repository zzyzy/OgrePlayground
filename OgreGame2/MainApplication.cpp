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
#include "SniperRifle.hpp"

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
	case OIS::KC_5:
		mWeapon.SetState(new SniperRifle(mSceneMgr, mCamera, &mBulletContext, mSniperScopeOverlay));
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
	if (!OgreContext::mouseMoved(me)) return false;
	if (!mFPSController.CaptureMouseMoved(me)) return false;
	return true;
}

bool MainApplication::mousePressed(const OIS::MouseEvent& me, OIS::MouseButtonID id)
{
	if (!OgreContext::mousePressed(me, id)) return false;
	if (!mFPSController.CaptureMousePressed(me, id)) return false;

	if (id == OIS::MB_Left)
	{
		mWeapon.handleInput(WeaponState::Input::MOUSEDOWN);
	} else if (id == OIS::MB_Right)
	{
		mWeapon.handleInput(WeaponState::Input::RIGHTMOUSEDOWN);
	}

	return true;
}

bool MainApplication::mouseReleased(const OIS::MouseEvent& me, OIS::MouseButtonID id)
{
	if (!OgreContext::mouseReleased(me, id)) return false;
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

	// Create a manual object for 2D
	Ogre::ManualObject* manual = mSceneMgr->createManualObject("manual");

	// Use identity view/projection matrices
	manual->setUseIdentityProjection(true);
	manual->setUseIdentityView(true);

	manual->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_LIST);

	manual->position(-0.05, 0.0, 0.0);
	manual->position(0.05, 0.0, 0.0);
	manual->position(0.0, -0.05, 0.0);
	manual->position(0.0, 0.05, 0.0);

	manual->end();

	// Use infinite AAB to always stay visible
	Ogre::AxisAlignedBox aabInf;
	aabInf.setInfinite();
	manual->setBoundingBox(aabInf);

	// Render just before overlays
	manual->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY - 1);

	// Attach to scene
	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(manual);

	// Get the pointer to the overlay manager
	Ogre::OverlayManager* overlayManager = Ogre::OverlayManager::getSingletonPtr();

	// Create an overlay
	mSniperScopeOverlay = overlayManager->create("Overlay");
	
	// Load an image into Ogre's texture manager
	Ogre::Image image;
	image.load("sniper-scope-cropped.png", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	Ogre::TextureManager::getSingleton().loadImage("LogoTexture", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, image, Ogre::TEX_TYPE_2D, 0, 1.0f);

	// Already loaded to texture, no longer need image
	image.freeMemory();

	// Create a material and assign the texture to the material
	Ogre::MaterialPtr materialPtr = Ogre::MaterialManager::getSingleton().create("ImageMaterial", "General");
	materialPtr->getTechnique(0)->getPass(0)->createTextureUnitState("LogoTexture");
	materialPtr->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SceneBlendType::SBT_TRANSPARENT_ALPHA);

	// Create a container and set it to the material we just defined
	Ogre::OverlayContainer* imageContainer = static_cast<Ogre::OverlayContainer*>(overlayManager->createOverlayElement("Panel", "DisplayImage1"));
	imageContainer->setMaterialName("ImageMaterial");
	imageContainer->setMetricsMode(Ogre::GMM_RELATIVE);
	imageContainer->setDimensions(1, 1);
	imageContainer->setPosition(0, 0);

	// Add the container to the overlay
	mSniperScopeOverlay->add2D(imageContainer);
	// Display the overlay
	//overlay->show();
}
