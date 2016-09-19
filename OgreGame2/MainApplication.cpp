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
#include "MachineGun.hpp"

MainApplication::MainApplication() :
	mSniperScopeOverlay(nullptr)
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
	case OIS::KC_3:
		mWeapon.SetState(new MachineGun(mSceneMgr, mCamera, &mBulletContext));
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

	// Setup ground, walls and ceiling (room)
	{
		// Setup graphics
		Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);
		Ogre::Plane wall(Ogre::Vector3::UNIT_Z, 0);

		Ogre::MeshManager::getSingleton().createPlane(
			"plane",
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			plane,
			100, 100, 20, 20,
			true,
			1, 5, 5,
			Ogre::Vector3::UNIT_Z
		);
		Ogre::MeshManager::getSingleton().createPlane(
			"wall",
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			wall,
			100, 40, 20, 20,
			true,
			1, 5, 5,
			Ogre::Vector3::UNIT_Y
		);

		// Ground
		auto *groundEntity = mSceneMgr->createEntity("plane");
		mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(groundEntity);
		groundEntity->setMaterialName("Examples/GrassFloor");

		// Ceiling
		auto *ceilingEntity = mSceneMgr->createEntity("plane");
		auto *ceilingNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		ceilingNode->attachObject(ceilingEntity);
		ceilingEntity->setMaterialName("Examples/BeachStones");
		ceilingNode->rotate(Ogre::Vector3::UNIT_X, Ogre::Degree(180));
		ceilingNode->translate(Ogre::Vector3(0, 40, 0));

		// Place walls
		Ogre::Entity *walls[4];
		Ogre::SceneNode *wallNodes[4];

		// Front wall
		walls[0] = mSceneMgr->createEntity("wall");
		wallNodes[0] = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		wallNodes[0]->attachObject(walls[0]);
		walls[0]->setMaterialName("Examples/Rockwall");
		wallNodes[0]->translate(Ogre::Vector3(0, 20, -50));

		// Right wall
		walls[1] = mSceneMgr->createEntity("wall");
		wallNodes[1] = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		wallNodes[1]->attachObject(walls[1]);
		walls[1]->setMaterialName("Examples/Rockwall");
		wallNodes[1]->rotate(Ogre::Vector3::UNIT_Y, Ogre::Degree(270));
		wallNodes[1]->translate(Ogre::Vector3(50, 20, 0));

		// Left wall
		walls[2] = mSceneMgr->createEntity("wall");
		wallNodes[2] = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		wallNodes[2]->attachObject(walls[2]);
		walls[2]->setMaterialName("Examples/Rockwall");
		wallNodes[2]->rotate(Ogre::Vector3::UNIT_Y, Ogre::Degree(90));
		wallNodes[2]->translate(Ogre::Vector3(-50, 20, 0));

		// Back wall
		walls[3] = mSceneMgr->createEntity("wall");
		wallNodes[3] = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		wallNodes[3]->attachObject(walls[3]);
		walls[3]->setMaterialName("Examples/Rockwall");
		wallNodes[3]->rotate(Ogre::Vector3::UNIT_Y, Ogre::Degree(180));
		wallNodes[3]->translate(Ogre::Vector3(0, 20, 50));
	
		// Setup rigidbodies
		btTransform transform;
		btVector3 origin;
		btQuaternion rotation;

		// Dummy entity for rigidbody
		Ogre::Entity* cube = mSceneMgr->createEntity("Ground", "cube.mesh");
		Ogre::SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		node->attachObject(cube);
		node->scale(1.0f, 0.1f, 1.0f);
		node->translate(0.0f, -5.0f, 0.0f);
		node->setVisible(false);

		// Ground rigidbody
		btBoxShape* groundShape = new btBoxShape(btVector3(50.0f, 5.0f, 50.0f));
		transform.setIdentity();
		transform.setOrigin(btVector3(0, -5, 0));
		mBulletContext.CreateRigidBody(0.0f, transform, groundShape, node);

		// Ceiling rigidbody
		btBoxShape* ceilingShape = new btBoxShape(btVector3(50.0f, 5.0f, 50.0f));
		origin = convert(ceilingNode->getPosition());
		rotation = convert(ceilingNode->getOrientation());
		transform.setIdentity();
		transform.setOrigin(btVector3(origin.x(), origin.y() + 5, origin.z()));
		transform.setRotation(rotation);
		mBulletContext.CreateRigidBody(0.0f, transform, ceilingShape, node);

		// Wall rigidbody
		btBoxShape* wallShape[4];
		// Front wall
		wallShape[0] = new btBoxShape(btVector3(50.0f, 20.0f, 5.0f));
		origin = convert(wallNodes[0]->getPosition());
		rotation = convert(wallNodes[0]->getOrientation());
		transform.setIdentity();
		transform.setOrigin(btVector3(origin.x(), origin.y(), origin.z() - 5));
		transform.setRotation(rotation);
		mBulletContext.CreateRigidBody(0.0f, transform, wallShape[0], node);

		// Right wall
		wallShape[1] = new btBoxShape(btVector3(50.0f, 20.0f, 5.0f));
		origin = convert(wallNodes[1]->getPosition());
		rotation = convert(wallNodes[1]->getOrientation());
		transform.setIdentity();
		transform.setOrigin(btVector3(origin.x() + 5, origin.y(), origin.z()));
		transform.setRotation(rotation);
		mBulletContext.CreateRigidBody(0.0f, transform, wallShape[1], node);

		// Left wall
		wallShape[2] = new btBoxShape(btVector3(50.0f, 20.0f, 5.0f));
		origin = convert(wallNodes[2]->getPosition());
		rotation = convert(wallNodes[2]->getOrientation());
		transform.setIdentity();
		transform.setOrigin(btVector3(origin.x() - 5, origin.y(), origin.z()));
		transform.setRotation(rotation);
		mBulletContext.CreateRigidBody(0.0f, transform, wallShape[2], node);

		// Back wall
		wallShape[3] = new btBoxShape(btVector3(50.0f, 20.0f, 5.0f));
		origin = convert(wallNodes[3]->getPosition());
		rotation = convert(wallNodes[3]->getOrientation());
		transform.setIdentity();
		transform.setOrigin(btVector3(origin.x(), origin.y(), origin.z() + 5));
		transform.setRotation(rotation);
		mBulletContext.CreateRigidBody(0.0f, transform, wallShape[3], node);
	}

	// Place objects in scene
	{
		// Create a row of boxes
		int boxCount = 0;

		for (int i = -20; i <= 20; i += 5)
		{
			char entityName[8];
			sprintf(entityName, "cube%d", boxCount++);

			// Create cube mesh with unique name
			Ogre::Entity* cube = mSceneMgr->createEntity(entityName, "cube.mesh");
			cube->setMaterialName("Examples/BumpyMetal");
			cube->setCastShadows(true);
			auto node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
			node->attachObject(cube);
			// Scale it to appropriate size
			node->scale(0.04, 0.04, 0.04);

			// Create a collision shape
			// Note that the size should match the size of the object that will be displayed
			btCollisionShape* collisionShape = new btBoxShape(btVector3(2, 2, 2));

			// The object's starting transformation
			btTransform startingTrans;
			startingTrans.setIdentity();
			startingTrans.setOrigin(btVector3(i, 10, 10));
			startingTrans.setRotation(btQuaternion(0, 0, 0, 1));

			// Create the rigid body
			btRigidBody* rigidBody = mBulletContext.CreateRigidBody(1.0f, startingTrans, collisionShape, node);

			// Set the rigid body's friction
			rigidBody->setFriction(0.9);
		}
	}

	// Setup FPS player
	{
		auto fpsPlayer = sceneMgr->getRootSceneNode()->createChildSceneNode("FPSPlayer");
		auto cameraNode = fpsPlayer->createChildSceneNode("CameraNode");
		cameraNode->attachObject(mCamera);
		mCamera->setPosition(0.0f, 0.0f, 0.0f);
		cameraNode->setPosition(0.0f, 10.0f, 0.0f);
		mFPSController.Attach(cameraNode, fpsPlayer);
		mWeapon.SetState(new BallShooter(mSceneMgr, mCamera, &mBulletContext));
	}

	// Draw crosshair
	{
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
	}

	// Setup overlay images
	{
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
	}
}
