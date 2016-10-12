//
// OgreGame3/MainApplication.cpp
// 
// Author: Zhen Zhi Lee <leezhenzhi@gmail.com>
//
// Copyright (C) 2016 Zhen Zhi Lee.
//

#include "MainApplication.hpp"
#include <random>

MainApplication::MainApplication() :
	mWorldGridNode(nullptr)
{
}

MainApplication::~MainApplication()
{
}

void MainApplication::Run()
{
	mPhysicsContext.Setup();
	mGraphicsContext.Setup("OgreGame3", this, this, this, this, this);
}

bool MainApplication::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	for (auto obj : mMovableObjects)
	{
		if (obj->HasSeekTarget())
		{
			obj->SeekTarget(&mWorld, mPathFinder);
		}

		obj->Update(4, evt.timeSinceLastFrame);
	}

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
	switch (ke.key)
	{
	case OIS::KC_0:
		mWorldGridNode->flipVisibility();
		for (auto obj : mMovableObjects)
		{
			obj->TogglePathVisualizer();
		}
		break;
	case OIS::KC_H:
		if (mGraphicsContext.GetTrayMgr()->isLogoVisible())
		{
			mGraphicsContext.GetTrayMgr()->hideFrameStats();
			mGraphicsContext.GetTrayMgr()->hideLogo();
		}
		else
		{
			mGraphicsContext.GetTrayMgr()->showFrameStats(OgreBites::TL_BOTTOMLEFT);
			mGraphicsContext.GetTrayMgr()->showLogo(OgreBites::TL_BOTTOMRIGHT);
		}
		break;
	default:
		break;
	}

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
	switch (id)
	{
	case OIS::MB_Right:
		{
			if (!mObjectSelector.HasSelection()) break;

			Ogre::Ray mouseRay = mGraphicsContext.GetMouseCursorRay();

			Ogre::RaySceneQuery* mRaySceneQuery = mGraphicsContext.CreateRayQuery();
			mRaySceneQuery->setRay(mouseRay);
			mRaySceneQuery->setSortByDistance(true);
			mRaySceneQuery->setQueryMask(ENVIRONMENT_MASK | ROBOT_MASK);

			Ogre::RaySceneQueryResult& result = mRaySceneQuery->execute();
			Ogre::RaySceneQueryResult::iterator itr = result.begin();

			if (!(itr != result.end() && itr->movable)) break;

			Ogre::Vector3 location = mouseRay.getPoint(itr->distance);

			if (itr->movable->getQueryFlags() == ROBOT_MASK)
			{
				for (auto obj : mObjectSelector.GetSelections())
				{
					if (itr->movable->getParentSceneNode()->getParentSceneNode() != obj)
					{
						std::cout << "Set seek target" << std::endl;
						obj->ClearPaths();
						obj->SetSeekTarget(itr->movable->getParentSceneNode()->getParentSceneNode());
					}
				}
			}
			else if (itr->movable->getQueryFlags() == ENVIRONMENT_MASK)
			{
				for (auto obj : mObjectSelector.GetSelections())
				{
					if (obj->HasSeekTarget())
					{
						obj->SetSeekTarget(nullptr);
					}

					obj->ClearPaths();

					int startNode = mWorld.getNode(obj->getPosition());
					int goalNode = mWorld.getNode(location);

					// check that goal node is not the same as start node
					if (goalNode != startNode)
					{
						// try to find path from start to goal node
						std::deque<int> path;

						// if path exists
						if (mPathFinder.AStar(startNode, goalNode, mWorld, path))
						{
							path.pop_front();

							for (auto p : path)
							{
								auto pos = mWorld.getPosition(p);
								pos.y = 1;
								obj->AddNode(pos);
							}

							obj->BuildPathVisualizer();
						}
					}
				}
			}
		}
		break;
	default:
		break;
	}

	if (!mRTSController.CaptureMouseReleased(me, id, mGraphicsContext.GetTrayMgr())) return false;
	if (!mObjectSelector.CaptureMouseReleased(me, id)) return false;
	if (!mGraphicsContext.CaptureMouseReleased(me, id)) return false;
	return true;
}

void MainApplication::SetupCamera(Ogre::SceneManager* const sceneMgr, Ogre::Camera* camera)
{
	auto cameraNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
	cameraNode->attachObject(camera);
	cameraNode->setPosition(0, 120, 0);
	cameraNode->lookAt(Ogre::Vector3(0, 0, 0), Ogre::Node::TS_WORLD);
	mRTSController.AttachCamera(cameraNode);
	mObjectSelector.Setup(sceneMgr, camera);
}

void MainApplication::SetupViewport(Ogre::RenderWindow* const window, Ogre::Camera* camera)
{
}

void MainApplication::SetupTrayUI(Ogre::SceneManager* const sceneMgr, OgreBites::SdkTrayManager* trayMgr)
{
	trayMgr->showFrameStats(OgreBites::TL_BOTTOMLEFT);
	trayMgr->showLogo(OgreBites::TL_BOTTOMRIGHT);
}

void MainApplication::SetupScene(Ogre::SceneManager* const sceneMgr, Ogre::Camera* camera)
{
	// Setup scene settings
	sceneMgr->setAmbientLight(Ogre::ColourValue(0.25f, 0.25f, 0.25f));
	sceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);

	// Setup light
	auto light = sceneMgr->createLight("MainLight");
	light->setType(Ogre::Light::LT_POINT);
	light->setDiffuseColour(Ogre::ColourValue::White);
	light->setSpecularColour(Ogre::ColourValue::White);
	light->setQueryFlags(0);

	auto lightNode = camera->getParentSceneNode()->createChildSceneNode();
	lightNode->attachObject(light);

    // Because cameraNode looks at the ground, the 'height' is the Z axis
    // Also, because the lightNode can be blocked by the cameraNode
    // so we displace it in negative units, to bring it to the
    // front of the cameraNode
	lightNode->setPosition(0, 0, -60);

	Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);
	Ogre::MeshManager::getSingleton().createPlane(
		"plane",
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		plane,
		GRID_DIMENSION * SQUARE_SIZE,
		GRID_DIMENSION * SQUARE_SIZE,
		20, 20,
		true,
		1, 2.5, 2.5,
		Ogre::Vector3::UNIT_Z
	);

	// Ground
	auto groundEntity = sceneMgr->createEntity("plane");
	groundEntity->setCastShadows(false);
	groundEntity->setQueryFlags(ENVIRONMENT_MASK);
	groundEntity->setMaterialName("Examples/GrassFloor");
	auto groundNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
	groundNode->attachObject(groundEntity);

	// Ground rigidbody
	btTransform groundTransform;
	btBoxShape* groundShape = new btBoxShape(btVector3((GRID_DIMENSION * SQUARE_SIZE) / 2.0f, 5, (GRID_DIMENSION * SQUARE_SIZE) / 2.0f));
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(0, -5, 0));
	auto groundRigidBody = mPhysicsContext.CreateRigidBody(0, groundTransform, groundShape, groundNode);
	groundRigidBody->setFriction(1);

	// Obstacles
	for (auto i = 0; i < TOTAL_NODES; ++i)
	{
		auto contents = mWorld.getContent(i);

		if (contents)
		{
			Ogre::Vector3 scale = Ogre::Vector3(0.1f, 0.1f, 0.1f);
			Ogre::Vector3 position = mWorld.getPosition(i);

			auto entity = sceneMgr->createEntity("cube.mesh");
			entity->setQueryFlags(ENVIRONMENT_MASK);
			entity->setCastShadows(true);
			entity->setMaterialName("Examples/FireScrolling");

			auto node = sceneMgr->getRootSceneNode()->createChildSceneNode();
			node->attachObject(entity);
			node->scale(scale);

			position.y = scale.y * 50.0f;
			node->translate(position);

			auto shape = new btBoxShape(btVector3(scale.x * 50.0f, scale.y * 50.0f, scale.z * 50.0f));
			btTransform transform;
			transform.setIdentity();
			transform.setOrigin(Convert(position));
			mPhysicsContext.CreateRigidBody(0, transform, shape, node);
		}
	}

	// Robots
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dis(0, TOTAL_NODES);
	int index = 0;
	std::unordered_set<int> indices;

	for (auto i = 0; i < 10; ++i)
	{
		auto content = mWorld.getContent(index);
		do
		{
			index = dis(gen);
			if (indices.find(index) != indices.end())
			{
				continue;
			}
			content = mWorld.getContent(index);
			indices.insert(index);
		}
		while (content != 0);

		auto scale = Ogre::Vector3(0.2f, 0.2f, 0.2f);
		auto position = mWorld.getPosition(index);

		auto entity = sceneMgr->createEntity("robot.mesh");
		entity->setCastShadows(true);
		entity->setQueryFlags(ROBOT_MASK);

		position.y = 2;
		auto node = OGRE_NEW MovableObject(sceneMgr, 25, Ogre::ColourValue(255 / 255.0f, 82 / 255.0f, 51 / 255.0f), entity);
		sceneMgr->getRootSceneNode()->addChild(node);
		auto child = node->createChildSceneNode();
		child->attachObject(entity);
		child->rotate(Ogre::Vector3::UNIT_Y, Ogre::Degree(-90));
		node->scale(scale);

		auto shape = new btBoxShape(btVector3(2, 2, 2));

		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(Convert(position));

		auto rigidBody = mPhysicsContext.CreateRigidBody(1, transform, shape, node);

		// Create a BillboardSet to represent a health bar and set its properties
		auto healthBar = sceneMgr->createBillboardSet();
		healthBar->setCastShadows(false);
		healthBar->setDefaultDimensions(30, 3);
		healthBar->setMaterialName("myMaterial/HealthBar");

		// Create a billboard for the health bar BillboardSet
		auto healthBarBB = healthBar->createBillboard(Ogre::Vector3(0, 100, 0));
		// Calculate the health bar adjustments
		float healthBarAdjuster = (1.0f - 1.0f) / 2; // This must range from 0.0 to 0.5
		// Set the health bar to the appropriate level
		healthBarBB->setTexcoordRect(0.0f + healthBarAdjuster, 0.0f, 0.5f + healthBarAdjuster, 1.0f);

		// Set it to always draw on top of other objects
		healthBar->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY);

		// Create a BillboardSet for a selection circle and set its properties
		auto selectionCircle = sceneMgr->createBillboardSet();
		selectionCircle->setCastShadows(false);
		selectionCircle->setDefaultDimensions(60, 60);
		selectionCircle->setMaterialName("myMaterial/SelectionCircle");
		selectionCircle->setBillboardType(Ogre::BillboardType::BBT_PERPENDICULAR_COMMON);
		selectionCircle->setCommonDirection(Ogre::Vector3(0, 1, 0));
		selectionCircle->setCommonUpVector(Ogre::Vector3(0, 0, -1));

		// Create a billboard for the selection circle BillboardSet
		auto selectionCircleBB = selectionCircle->createBillboard(Ogre::Vector3(0, 1, 0));
		selectionCircleBB->setTexcoordRect(0, 0, 1, 1);

		node->AttachDecal(healthBar);
		node->AttachDecal(selectionCircle);
		node->SetRigidBody(rigidBody);
		mMovableObjects.push_back(node);
	}

	// Build world grid
	auto worldGridColour = Ogre::ColourValue(51 / 255.0f, 255 / 255.0f, 247 / 255.0f);
	auto worldGrid = sceneMgr->createManualObject();
	worldGrid->clear();
	worldGrid->setQueryFlags(0);
	for (auto i = 0; i < TOTAL_NODES; ++i)
	{
		worldGrid->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_STRIP);
		auto position = mWorld.getPosition(i);
		position.x -= SQUARE_SIZE / 2.0f;
		position.z -= SQUARE_SIZE / 2.0f;
		position.y = 1;
		worldGrid->position(position);
		worldGrid->colour(worldGridColour);
		position.x += SQUARE_SIZE;
		worldGrid->position(position);
		worldGrid->colour(worldGridColour);
		position.z += SQUARE_SIZE;
		worldGrid->position(position);
		worldGrid->colour(worldGridColour);
		position.x -= SQUARE_SIZE;
		worldGrid->position(position);
		worldGrid->colour(worldGridColour);
		worldGrid->end();
	}
	mWorldGridNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
	mWorldGridNode->attachObject(worldGrid);
	mWorldGridNode->setVisible(false);
}
