//
// OgreGame2/BulletContext.cpp
//
// Author: Zhen Zhi Lee <leezhenzhi@gmail.com>
//
// Copyright (C) 2016 Zhen Zhi Lee.
//

#include "BulletContext.hpp"

BulletContext::BulletContext() :
    mCollisionConfiguration(nullptr),
    mDispatcher(nullptr),
    mBroadphase(nullptr),
    mSolver(nullptr),
    mDynamicsWorld(nullptr)
{
}

BulletContext::~BulletContext()
{
    Teardown();
}

void BulletContext::Setup()
{
    ///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
    mCollisionConfiguration = new btDefaultCollisionConfiguration();

    ///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
    mBroadphase = new btDbvtBroadphase();

    ///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
    mDispatcher = new btCollisionDispatcher(mCollisionConfiguration);

    ///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
    mSolver = new btSequentialImpulseConstraintSolver;

    // Create a dynamic simulation environment
    mDynamicsWorld = new btDiscreteDynamicsWorld(mDispatcher, mBroadphase, mSolver, mCollisionConfiguration);

    // Set the gravity for the simulation
    mDynamicsWorld->setGravity(btVector3(0, -10, 0));
}

void BulletContext::Teardown()
{
    if (mDynamicsWorld == nullptr)
    {
        std::cerr << "Object already disposed" << std::endl;
        return;
    }
    //cleanup in the reverse order of creation/initialization

    //remove the rigidbodies from the dynamics world and delete them
    for (auto i = mDynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
    {
        btCollisionObject* obj = mDynamicsWorld->getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast(obj);
        if (body && body->getMotionState())
        {
            delete body->getMotionState();
        }
        mDynamicsWorld->removeCollisionObject(obj);
        delete obj;
    }

    //delete collision shapes
    for (auto j = 0; j < mCollisionShapes.size(); j++)
    {
        btCollisionShape* shape = mCollisionShapes[j];
        delete shape;
    }
    mCollisionShapes.clear();

    delete mDynamicsWorld;
    delete mSolver;
    delete mBroadphase;
    delete mDispatcher;
    delete mCollisionConfiguration;
    mDynamicsWorld = nullptr;
    mSolver = nullptr;
    mBroadphase = nullptr;
    mDispatcher = nullptr;
    mCollisionConfiguration = nullptr;
}

void BulletContext::Update(float deltaTime) const
{
    // Simulate dynamics
    mDynamicsWorld->stepSimulation(deltaTime);
}

btRigidBody* BulletContext::CreateRigidBody(float mass, const btTransform& startTrans, btCollisionShape* collisionShape, Ogre::SceneNode* node)
{
    // Check validity of collision shape
    btAssert((!collisionShape || collisionShape->getShapeType() != INVALID_SHAPE_PROXYTYPE));

    // Push the collision shape into the collision shapes array
    mCollisionShapes.push_back(collisionShape);

    // Rigidbody is dynamic if and only if mass is non zero, otherwise it's static
    bool isDynamic = (mass != 0.f);

    // Calculate moment of inertia
    btVector3 inertia(0, 0, 0);
    if (isDynamic)
        collisionShape->calculateLocalInertia(mass, inertia);

    // Create rigid body
    btMotionState* motionState = new OgreMotionState(startTrans, node);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, collisionShape, inertia);
    btRigidBody* rigidBody = new btRigidBody(rbInfo);

    // Add rigid body to dynamic simulation
    mDynamicsWorld->addRigidBody(rigidBody);

    return rigidBody;
}
