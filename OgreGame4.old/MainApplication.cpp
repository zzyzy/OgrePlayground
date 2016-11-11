//
// OgreGame3/MainApplication.cpp
// 
// Author: Zhen Zhi Lee <leezhenzhi@gmail.com>
//
// Copyright (C) 2016 Zhen Zhi Lee.
//

#include "MainApplication.hpp"
#include <random>
#include "OgreEuler.hpp"
#include "CollisionMasks.hpp"

MainApplication::MainApplication() :
    mWorldGridNode(nullptr),
    mBarrel(nullptr)
{
}

MainApplication::~MainApplication()
{
    delete mBarrel;
}

void MainApplication::Run()
{
    mPhysicsContext.Setup();
    mGraphicsContext.Setup("OgreGame4", this, this, this, this, this);
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

    //mBarrel->FireAt(Ogre::Vector3(100, 0, 100));

    mBarrel->Update(evt.timeSinceLastFrame);

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
            mRaySceneQuery->setQueryMask(ENVIRONMENT_MASK);

            Ogre::RaySceneQueryResult& result = mRaySceneQuery->execute();
            Ogre::RaySceneQueryResult::iterator itr = result.begin();

            if (!(itr != result.end() && itr->movable)) break;

            Ogre::Vector3 location = mouseRay.getPoint(itr->distance);
            std::cout << location << std::endl;

            mBarrel->FireAt(location);
            //break;

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
    cameraNode->setPosition(0, 150, 0);
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
    lightNode->setPosition(0, 0, -10);

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
    auto groundRigidBody = mPhysicsContext.CreateRigidBody(0, groundTransform, groundShape, groundNode,
                                                           COL_GROUND,
                                                           COL_TANK |
                                                           COL_POWERUP |
                                                           COL_PROJECTILE |
                                                           COL_ENVIRONMENT_OBJECT);
    groundRigidBody->setFriction(1);

    //// Obstacles
    //for (auto i = 0; i < TOTAL_NODES; ++i)
    //{
    //	auto contents = mWorld.getContent(i);

    //	if (contents)
    //	{
    //		Ogre::Vector3 scale = Ogre::Vector3(0.1f, 0.1f, 0.1f);
    //		Ogre::Vector3 position = mWorld.getPosition(i);

    //		auto entity = sceneMgr->createEntity("cube.mesh");
    //		entity->setQueryFlags(ENVIRONMENT_MASK);
    //		entity->setCastShadows(true);
    //		entity->setMaterialName("Examples/FireScrolling");

    //		auto node = sceneMgr->getRootSceneNode()->createChildSceneNode();
    //		node->attachObject(entity);
    //		node->scale(scale);

    //		position.y = scale.y * 50.0f;
    //		node->translate(position);

    //		auto shape = new btBoxShape(btVector3(scale.x * 50.0f, scale.y * 50.0f, scale.z * 50.0f));
    //		btTransform transform;
    //		transform.setIdentity();
    //		transform.setOrigin(Convert(position));
    //		mPhysicsContext.CreateRigidBody(0, transform, shape, node);
    //	}
    //}

    //// Robots
    //std::random_device rd;
    //std::mt19937 gen(rd());
    //std::uniform_int_distribution<int> dis(0, TOTAL_NODES);
    //int index = 0;
    //std::unordered_set<int> indices;

    //for (auto i = 0; i < 10; ++i)
    //{
    //	auto content = mWorld.getContent(index);
    //	do
    //	{
    //		index = dis(gen);
    //		if (indices.find(index) != indices.end())
    //		{
    //			continue;
    //		}
    //		content = mWorld.getContent(index);
    //		indices.insert(index);
    //	}
    //	while (content != 0);

    //	auto scale = Ogre::Vector3(0.2f, 0.2f, 0.2f);
    //	auto position = mWorld.getPosition(index);

    //	auto entity = sceneMgr->createEntity("robot.mesh");
    //	entity->setCastShadows(true);
    //	entity->setQueryFlags(ROBOT_MASK);

    //	position.y = 2;
    //	auto node = OGRE_NEW MovableObject(sceneMgr, 25, Ogre::ColourValue(255 / 255.0f, 82 / 255.0f, 51 / 255.0f), entity);
    //	sceneMgr->getRootSceneNode()->addChild(node);
    //	auto child = node->createChildSceneNode();
    //	child->attachObject(entity);
    //	child->rotate(Ogre::Vector3::UNIT_Y, Ogre::Degree(-90));
    //	node->scale(scale);

    //	auto shape = new btBoxShape(btVector3(2, 2, 2));

    //	btTransform transform;
    //	transform.setIdentity();
    //	transform.setOrigin(Convert(position));

    //	auto rigidBody = mPhysicsContext.CreateRigidBody(1, transform, shape, node);

    //	// Create a BillboardSet to represent a health bar and set its properties
    //	auto healthBar = sceneMgr->createBillboardSet();
    //	healthBar->setCastShadows(false);
    //	healthBar->setDefaultDimensions(30, 3);
    //	healthBar->setMaterialName("myMaterial/HealthBar");

    //	// Create a billboard for the health bar BillboardSet
    //	auto healthBarBB = healthBar->createBillboard(Ogre::Vector3(0, 100, 0));
    //	// Calculate the health bar adjustments
    //	float healthBarAdjuster = (1.0f - 1.0f) / 2; // This must range from 0.0 to 0.5
    //	// Set the health bar to the appropriate level
    //	healthBarBB->setTexcoordRect(0.0f + healthBarAdjuster, 0.0f, 0.5f + healthBarAdjuster, 1.0f);

    //	// Set it to always draw on top of other objects
    //	healthBar->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY);

    //	// Create a BillboardSet for a selection circle and set its properties
    //	auto selectionCircle = sceneMgr->createBillboardSet();
    //	selectionCircle->setCastShadows(false);
    //	selectionCircle->setDefaultDimensions(60, 60);
    //	selectionCircle->setMaterialName("myMaterial/SelectionCircle");
    //	selectionCircle->setBillboardType(Ogre::BillboardType::BBT_PERPENDICULAR_COMMON);
    //	selectionCircle->setCommonDirection(Ogre::Vector3(0, 1, 0));
    //	selectionCircle->setCommonUpVector(Ogre::Vector3(0, 0, -1));

    //	// Create a billboard for the selection circle BillboardSet
    //	auto selectionCircleBB = selectionCircle->createBillboard(Ogre::Vector3(0, 1, 0));
    //	selectionCircleBB->setTexcoordRect(0, 0, 1, 1);

    //	node->AttachDecal(healthBar);
    //	node->AttachDecal(selectionCircle);
    //	node->SetRigidBody(rigidBody);
    //	mMovableObjects.push_back(node);
    //}

    {
    	auto scale = Ogre::Vector3(0.2f, 0.2f, 0.2f);
    	auto position = mWorld.getPosition(mWorld.getNode(Ogre::Vector3(0, 0, 0)));

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

    // Place objects in scene
    {
        // Create a row of boxes
        for (int i = -20; i <= 20; i += 5)
        {
            // Create cube mesh with unique name
            Ogre::Entity* cube = sceneMgr->createEntity("cube.mesh");
            cube->setMaterialName("Examples/BumpyMetal");
            cube->setCastShadows(true);
            cube->setQueryFlags(0);
            auto node = sceneMgr->getRootSceneNode()->createChildSceneNode();
            node->attachObject(cube);
            // Scale it to appropriate size
            node->scale(0.04f, 0.04f, 0.04f);

            // Create a collision shape
            // Note that the size should match the size of the object that will be displayed
            btCollisionShape* collisionShape = new btBoxShape(btVector3(2, 2, 2));

            // The object's starting transformation
            btTransform startingTrans;
            startingTrans.setIdentity();
            startingTrans.setOrigin(btVector3(i, 10, 10));
            startingTrans.setRotation(btQuaternion(0, 0, 0, 1));

            // Create the rigid body
            btRigidBody* rigidBody = mPhysicsContext.CreateRigidBody(1.0f, startingTrans, collisionShape, node,
                                                                     COL_ENVIRONMENT_OBJECT,
                                                                     COL_TANK |
                                                                     COL_GROUND |
                                                                     COL_EXPLOSION |
                                                                     COL_PROJECTILE |
                                                                     COL_ENVIRONMENT_OBJECT);

            // Set the rigid body's friction
            rigidBody->setFriction(0.9f);
        }

        for (int i = -20; i <= 20; i += 5)
        {
            // Create cube mesh with unique name
            Ogre::Entity* cube = sceneMgr->createEntity("cube.mesh");
            cube->setMaterialName("Examples/Checker");
            cube->setCastShadows(true);
            cube->setQueryFlags(0);
            auto node = sceneMgr->getRootSceneNode()->createChildSceneNode();
            node->attachObject(cube);
            // Scale it to appropriate size
            node->scale(0.06f, 0.06f, 0.06f);

            // Create a collision shape
            // Note that the size should match the size of the object that will be displayed
            btCollisionShape* collisionShape = new btBoxShape(btVector3(3, 3, 3));

            // The object's starting transformation
            btTransform startingTrans;
            startingTrans.setIdentity();
            startingTrans.setOrigin(btVector3(i, 10, -10));
            startingTrans.setRotation(btQuaternion(0, 0, 0, 1));

            // Create the rigid body
            btRigidBody* rigidBody = mPhysicsContext.CreateRigidBody(2.0f, startingTrans, collisionShape, node,
                                                                     COL_ENVIRONMENT_OBJECT,
                                                                     COL_TANK |
                                                                     COL_GROUND |
                                                                     COL_EXPLOSION |
                                                                     COL_PROJECTILE |
                                                                     COL_ENVIRONMENT_OBJECT);

            // Set the rigid body's friction
            rigidBody->setFriction(1.8f);
        }

        for (int i = -20; i <= 20; i += 5)
        {
            // Create cube mesh with unique name
            Ogre::Entity* cube = sceneMgr->createEntity("cube.mesh");
            cube->setMaterialName("Examples/TextureEffect3");
            cube->setCastShadows(true);
            cube->setQueryFlags(0);
            auto node = sceneMgr->getRootSceneNode()->createChildSceneNode();
            node->attachObject(cube);
            // Scale it to appropriate size
            node->scale(0.08f, 0.08f, 0.08f);

            // Create a collision shape
            // Note that the size should match the size of the object that will be displayed
            btCollisionShape* collisionShape = new btBoxShape(btVector3(4, 4, 4));

            // The object's starting transformation
            btTransform startingTrans;
            startingTrans.setIdentity();
            startingTrans.setOrigin(btVector3(i, 10, -20));
            startingTrans.setRotation(btQuaternion(0, 0, 0, 1));

            // Create the rigid body
            btRigidBody* rigidBody = mPhysicsContext.CreateRigidBody(3.0f, startingTrans, collisionShape, node,
                                                                     COL_ENVIRONMENT_OBJECT,
                                                                     COL_TANK |
                                                                     COL_GROUND |
                                                                     COL_EXPLOSION |
                                                                     COL_PROJECTILE |
                                                                     COL_ENVIRONMENT_OBJECT);

            // Set the rigid body's friction
            rigidBody->setFriction(2.7f);
        }
    }

    // Tank ch
    {
        auto bodyEntity = sceneMgr->createEntity("chbody.mesh");
        auto bodyNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
        auto bodyChild = bodyNode->createChildSceneNode();
        bodyEntity->setCastShadows(true);
        bodyEntity->setMaterialName("ch_tank_material");
        bodyChild->attachObject(bodyEntity);
        bodyChild->rotate(Ogre::Vector3::UNIT_Y, Ogre::Degree(90));

        auto turretEntity = sceneMgr->createEntity("chturret.mesh");
        auto turretNode = bodyNode->createChildSceneNode();
        auto turretChild = turretNode->createChildSceneNode();
        turretEntity->setCastShadows(true);
        turretEntity->setMaterialName("ch_tank_material");
        turretChild->attachObject(turretEntity);
        turretChild->rotate(Ogre::Vector3::UNIT_Y, Ogre::Degree(90));
        turretNode->translate(0, 3, 0);

        auto barrelEntity = sceneMgr->createEntity("chbarrel.mesh");
        auto barrelNode = turretNode->createChildSceneNode();
        auto barrelChild = barrelNode->createChildSceneNode();
        barrelEntity->setCastShadows(true);
        barrelEntity->setMaterialName("ch_tank_material");
        barrelChild->attachObject(barrelEntity);
        barrelChild->rotate(Ogre::Vector3::UNIT_Y, Ogre::Degree(90));
        barrelNode->translate(0, 12, 36);

        auto nozzleEntity = sceneMgr->createEntity("sphere.mesh");
        auto nozzleNode = barrelNode->createChildSceneNode();
        auto nozzleChild = nozzleNode->createChildSceneNode();
        nozzleEntity->setCastShadows(true);
        nozzleChild->attachObject(nozzleEntity);
        nozzleChild->scale(0.05f, 0.05f, 0.05f);
        nozzleNode->translate(0, 0, 60);

        nozzleNode->setVisible(false);
        bodyNode->scale(0.25f, 0.25f, 0.25f);
        bodyNode->scale(0.5f, 0.5f, 0.5f);
        bodyNode->translate(-70, 5, -50);
    }

    // Tank lp
    {
        auto bodyEntity = sceneMgr->createEntity("lpbody.mesh");
        auto bodyNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
        auto bodyChild = bodyNode->createChildSceneNode();
        bodyEntity->setCastShadows(true);
        bodyEntity->setMaterialName("lp_tank_material");
        bodyChild->attachObject(bodyEntity);
        bodyChild->rotate(Ogre::Vector3::UNIT_Y, Ogre::Degree(90));

        auto turretEntity = sceneMgr->createEntity("lpturret.mesh");
        auto turretNode = bodyNode->createChildSceneNode();
        auto turretChild = turretNode->createChildSceneNode();
        turretEntity->setCastShadows(true);
        turretEntity->setMaterialName("lp_tank_material");
        turretChild->attachObject(turretEntity);
        turretChild->rotate(Ogre::Vector3::UNIT_Y, Ogre::Degree(90));
        turretNode->translate(0, 1, 0);

        auto barrelEntity = sceneMgr->createEntity("lpbarrel.mesh");
        auto barrelNode = turretNode->createChildSceneNode();
        auto barrelChild = barrelNode->createChildSceneNode();
        barrelEntity->setCastShadows(true);
        barrelEntity->setMaterialName("lp_tank_material");
        barrelChild->attachObject(barrelEntity);
        barrelChild->rotate(Ogre::Vector3::UNIT_Y, Ogre::Degree(90));
        barrelNode->translate(-1.25f, 12, 30);

        auto nozzleEntity = sceneMgr->createEntity("sphere.mesh");
        auto nozzleNode = barrelNode->createChildSceneNode();
        auto nozzleChild = nozzleNode->createChildSceneNode();
        nozzleEntity->setCastShadows(true);
        nozzleChild->attachObject(nozzleEntity);
        nozzleChild->scale(0.05f, 0.05f, 0.05f);
        nozzleNode->translate(0.75f, 0, 40);

        nozzleNode->setVisible(false);
        bodyNode->scale(0.33f, 0.33f, 0.33f);
        bodyNode->scale(0.5f, 0.5f, 0.5f);
        bodyNode->translate(-50, 5, -50);

        mBarrel = new Barrel(turretNode, barrelNode, nozzleNode, sceneMgr, &mPhysicsContext, 5, 1, 250, 2, 1.0f, 10);
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
