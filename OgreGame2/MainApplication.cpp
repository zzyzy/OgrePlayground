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

    if (id == OIS::MB_Left)
    {
        // Get the mouse ray, i.e. ray from the mouse cursor 'into' the screen 
        Ogre::Ray mouseRay = mCamera->getCameraToViewportRay(
            static_cast<float>(mMouse->getMouseState().X.abs) / mMouse->getMouseState().width,
            static_cast<float>(mMouse->getMouseState().Y.abs) / mMouse->getMouseState().height);

        // Get a point one unit from the mouse ray origin, in the direction of the ray
        Ogre::Vector3 destination = mouseRay.getPoint(1);

        // Calculate the direction for the linear velocity
        btVector3 linearVelocity(
            destination.x - mouseRay.getOrigin().x,
            destination.y - mouseRay.getOrigin().y,
            destination.z - mouseRay.getOrigin().z);

        linearVelocity.normalize();
        // Scale to appropriate velocity
        linearVelocity *= 30.0f;

        // Create and shoot the box
        shootBox(convert(mouseRay.getOrigin()), btQuaternion(0, 0, 0, 1), linearVelocity);
    }

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
    /*Ogre::MeshManager::getSingleton().createPlane(
        "plane",
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        plane,
        500, 500, 20, 20,
        true,
        1, 5, 5,
        Ogre::Vector3::UNIT_Z
    );*/
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
    mFPSController.Attach(cameraNode, fpsPlayer);
}

void MainApplication::shootBox(const btVector3& position, const btQuaternion& orientation, const btVector3& linearVelocity)
{
    // Create unique name
    std::ostringstream oss;
    oss << mBoxCount;
    std::string entityName = "Cube" + oss.str();
    // Increment box count
    mBoxCount++;

    // Create cube mesh with unique name
    Ogre::Entity* cube = mSceneMgr->createEntity(entityName, "cube.mesh");
    Ogre::SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    node->attachObject(cube);
    // Scale it to appropriate size
    node->scale(0.02, 0.02, 0.02);

    // Create a collision shape
    // Note that the size should match the size of the object that will be displayed
    btCollisionShape* collisionShape = new btBoxShape(btVector3(1, 1, 1));

    // The object's starting transformation
    btTransform startingTrans;
    startingTrans.setIdentity();
    startingTrans.setOrigin(position);
    startingTrans.setRotation(orientation);

    // Create the rigid body
    btRigidBody* rigidBody = mBulletContext.CreateRigidBody(1.0f, startingTrans, collisionShape, node);

    // Give the rigid body an initial velocity
    rigidBody->setLinearVelocity(linearVelocity);
}
