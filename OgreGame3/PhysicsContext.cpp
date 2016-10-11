//
// OgreGame3/PhysicsContext.cpp
//
// Author: Zhen Zhi Lee <leezhenzhi@gmail.com>
//
// Copyright (C) 2016 Zhen Zhi Lee.
//

#include "PhysicsContext.hpp"

PhysicsContext::PhysicsContext() :
	mCollisionConfiguration(nullptr),
	mDispatcher(nullptr),
	mBroadphase(nullptr),
	mSolver(nullptr),
	mDynamicsWorld(nullptr)
{
	Setup();
}

PhysicsContext::~PhysicsContext()
{
	Teardown();
}

void PhysicsContext::Setup()
{
	// Collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	mCollisionConfiguration = new btDefaultCollisionConfiguration();

	// btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	mBroadphase = new btDbvtBroadphase();

	// Use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	mDispatcher = new btCollisionDispatcher(mCollisionConfiguration);

	// The default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	mSolver = new btSequentialImpulseConstraintSolver;

	// Create a dynamic simulation environment
	mDynamicsWorld = new btDiscreteDynamicsWorld(mDispatcher, mBroadphase, mSolver, mCollisionConfiguration);

	// Set the gravity for the simulation
	mDynamicsWorld->setGravity(btVector3(0, -10, 0));

	// Setup for ghost object collision detection
	mBroadphase->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback);
}

void PhysicsContext::Teardown()
{
	// Cleanup in the reverse order of creation/initialization
	// Remove the rigidbodies from the dynamics world and delete them
	for (auto i = mDynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		auto obj = mDynamicsWorld->getCollisionObjectArray()[i];
		auto body = btRigidBody::upcast(obj);

		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}

		mDynamicsWorld->removeCollisionObject(obj);
		delete obj;
	}

	// Delete collision shapes
	for (auto j = 0; j < mCollisionShapes.size(); j++)
	{
		auto shape = mCollisionShapes[j];
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

void PhysicsContext::Update(const float& deltaTime) const
{
	// Simulate dynamics
	mDynamicsWorld->stepSimulation(deltaTime);
}

btRigidBody* PhysicsContext::CreateRigidBody(const float& mass, const btTransform& startTrans, btCollisionShape* collisionShape, Ogre::SceneNode* node)
{
	// Check validity of collision shape
	//btAssert((!collisionShape || collisionShape->getShapeType() != INVALID_SHAPE_PROXYTYPE));

	// Push the collision shape into the collision shapes array
	mCollisionShapes.push_back(collisionShape);

	// Rigidbody is dynamic if and only if mass is non zero, otherwise it's static
	auto isDynamic = (mass != 0.f);

	// Calculate moment of inertia
	btVector3 inertia(0, 0, 0);
	if (isDynamic)
	{
		collisionShape->calculateLocalInertia(mass, inertia);
	}

	// Create rigid body
	btMotionState* motionState = new MotionState(startTrans, node);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, collisionShape, inertia);
	btRigidBody* rigidBody = new btRigidBody(rbInfo);

	// Add rigid body to dynamic simulation
	mDynamicsWorld->addRigidBody(rigidBody);

	return rigidBody;
}

void PhysicsContext::DestroyRigidBody(btRigidBody* rigidBody)
{
	// Get the collision shape
	auto shape = rigidBody->getCollisionShape();

	// Remove the collision shape from the list of collision objects
	mCollisionShapes.remove(shape);

	// Remove the rigid body from collision and rigidbody computations
	mDynamicsWorld->removeCollisionObject(rigidBody);
	mDynamicsWorld->removeRigidBody(rigidBody);

	// Delete the objects
	delete shape;
	delete static_cast<MotionState*>(rigidBody->getMotionState());
	delete rigidBody;
}

btPairCachingGhostObject* PhysicsContext::CreateGhostObject(const btTransform& startTrans,
                                                           btCollisionShape* collisionShape,
                                                           btBroadphaseProxy::CollisionFilterGroups filterGroup,
                                                           btBroadphaseProxy::CollisionFilterGroups filterMask)
{
	mCollisionShapes.push_back(collisionShape);
	auto ghostObject = new btPairCachingGhostObject();
	ghostObject->setWorldTransform(startTrans);
	ghostObject->setCollisionShape(collisionShape);
	mDynamicsWorld->addCollisionObject(ghostObject, filterGroup, filterMask);
	return ghostObject;
}

void PhysicsContext::DestroyGhostObject(btPairCachingGhostObject* ghostObject)
{
	// Get the collision shape
	auto shape = ghostObject->getCollisionShape();

	// Remove the collision shape from the list of collision objects
	mCollisionShapes.remove(shape);

	// Remove the collision object from the world
	mDynamicsWorld->removeCollisionObject(ghostObject);

	delete shape;
	delete ghostObject;
}

btCollisionObject* PhysicsContext::GetCollidedObject(btCollisionObject* object) const
{
	const auto numManifolds = mDynamicsWorld->getDispatcher()->getNumManifolds();
	for (auto i = 0; i < numManifolds; ++i)
	{
		btPersistentManifold* manifold = mDynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
		btCollisionObject* objA = const_cast<btCollisionObject*>(manifold->getBody0());
		btCollisionObject* objB = const_cast<btCollisionObject*>(manifold->getBody1());

		if (objA == object || objB == object)
		{
			const auto numContacts = manifold->getNumContacts();
			for (auto j = 0; j < numContacts; ++j)
			{
				btManifoldPoint& pt = manifold->getContactPoint(j);

				if (pt.getDistance() < 0.f)
				{
					if (objA == object)
						return objB;
					else
						return objA;
				}
			}
		}
	}

	return nullptr;
}

std::set<btCollisionObject*> PhysicsContext::GetAllCollidedObjects(btCollisionObject* object) const
{
	std::set<btCollisionObject*> collidedObjects;

	const auto numManifolds = mDynamicsWorld->getDispatcher()->getNumManifolds();
	for (auto i = 0; i < numManifolds; ++i)
	{
		btPersistentManifold* manifold = mDynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
		btCollisionObject* objA = const_cast<btCollisionObject*>(manifold->getBody0());
		btCollisionObject* objB = const_cast<btCollisionObject*>(manifold->getBody1());

		if (objA == object || objB == object)
		{
			const auto numContacts = manifold->getNumContacts();
			for (auto j = 0; j < numContacts; ++j)
			{
				btManifoldPoint& pt = manifold->getContactPoint(j);

				if (pt.getDistance() < 0.f)
				{
					if (objA == object)
						collidedObjects.insert(objB);
					else
						collidedObjects.insert(objA);
				}
			}
		}
	}

	return collidedObjects;
}

std::set<btCollisionObject*> PhysicsContext::GetAllCollidedObjects(btPairCachingGhostObject* object) const
{
	std::set<btCollisionObject*> collidedObjects;
	btManifoldArray manifoldArray;
	btBroadphasePairArray& pairArray = object->getOverlappingPairCache()->getOverlappingPairArray();

	for (auto i = 0; i < pairArray.size(); ++i)
	{
		manifoldArray.clear();

		const btBroadphasePair& pair = pairArray[i];
		btBroadphasePair* collisionPair = mDynamicsWorld->getPairCache()->findPair(pair.m_pProxy0, pair.m_pProxy1);

		if (!collisionPair) continue;

		if (collisionPair->m_algorithm)
		{
			collisionPair->m_algorithm->getAllContactManifolds(manifoldArray);
		}

		for (auto j = 0; j < manifoldArray.size(); ++j)
		{
			btPersistentManifold* manifold = manifoldArray[j];
			btCollisionObject* objA = const_cast<btCollisionObject*>(manifold->getBody0());
			btCollisionObject* objB = const_cast<btCollisionObject*>(manifold->getBody1());

			if (objA == object || objB == object)
			{
				const auto numContacts = manifold->getNumContacts();
				for (auto k = 0; k < numContacts; k++)
				{
					btManifoldPoint& pt = manifold->getContactPoint(k);

					if (pt.getDistance() < 0.f)
					{
						if (objA == object)
							collidedObjects.insert(objB);
						else
							collidedObjects.insert(objA);
					}
				}
			}
		}
	}

	return collidedObjects;
}

void PhysicsContext::ShootRay(const btVector3& rayFromWorld, const btVector3& rayToWorld, const btVector3& impulse) const
{
	// Create a closest ray result callback
	btCollisionWorld::ClosestRayResultCallback rayCallback(rayFromWorld, rayToWorld);

	// Shoot a ray given a start and end position and fills in the ray callback
	mDynamicsWorld->rayTest(rayFromWorld, rayToWorld, rayCallback);

	// Check whether the ray hit anything
	if (rayCallback.hasHit())
	{
		// Get the object that the ray hit
		btRigidBody* body = const_cast<btRigidBody*>(btRigidBody::upcast(rayCallback.m_collisionObject));

		// If the rigid body exists
		if (body)
		{
			// Activate the object dynamics
			body->setActivationState(ACTIVE_TAG);

			// Compute the direction to apply the impulse
			btVector3 relPos = rayCallback.m_hitPointWorld - body->getCenterOfMassPosition();

			// Apply the impulse
			body->applyImpulse(impulse, relPos);
		}
	}
}
