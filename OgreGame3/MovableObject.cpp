//
// OgreGame3/MovableObject.cpp
//
// Author: Zhen Zhi Lee <leezhenzhi@gmail.com>
//
// Copyright (C) 2016 Zhen Zhi Lee.
//

#include "MovableObject.hpp"
#include "Utilities.hpp"

MovableObject::MovableObject(Ogre::SceneManager* creator,
                             const Ogre::Real& moveSpeed,
                             const Ogre::ColourValue& pathColour,
                             Ogre::Entity* entity) :
	SceneNode(creator),
	mRigidBody(nullptr),
	mPathVisualizer(nullptr),
	mPathVizColour(pathColour),
	mMoveSpeed(moveSpeed),
	mDistance(0.0f),
	mDestination(Ogre::Vector3::ZERO),
	mDirection(Ogre::Vector3::ZERO),
	mDecal(nullptr),
	mSeekTarget(nullptr),
	mEntity(entity),
	mAnimationState(nullptr),
	mShowPathViz(false)
{
	Setup(creator);
}

void MovableObject::Setup(Ogre::SceneManager* creator)
{
	mPathVisualizer = creator->createManualObject();
	mPathVisualizer->clear();
	mPathVisualizer->setDynamic(true);
	mPathVisualizer->setQueryFlags(0);
	creator->getRootSceneNode()->createChildSceneNode()->attachObject(mPathVisualizer);
	mPathVisualizer->getParentSceneNode()->setVisible(mShowPathViz);

	// TODO Let decal be in one scenenode and the entity/model in another
	//	    so that scaling doesn't affect the decal
	mDecal = createChildSceneNode();
	mDecal->setVisible(false);

	mAnimationState = mEntity->getAnimationState("Idle");
	mAnimationState->setLoop(true);
	mAnimationState->setEnabled(true);
}

void MovableObject::SetRigidBody(btRigidBody* rigidBody)
{
	mRigidBody = rigidBody;
}

btRigidBody* MovableObject::GetRigidBody() const
{
	return mRigidBody;
}

void MovableObject::SetPath(const std::deque<Ogre::Vector3>& path)
{
	mPath = path;
}

void MovableObject::AddNode(const Ogre::Vector3& location)
{
	mPath.push_back(location);
}

bool MovableObject::IsPathEmpty() const
{
	return mPath.empty();
}

Ogre::Vector3 MovableObject::GetLastNode() const
{
	return mPath.back();
}

Ogre::Vector3 MovableObject::Seek(const Ogre::Vector3& from, const Ogre::Vector3& target,
                                  const float& speed, const Ogre::Vector3& currentVelocity)
{
	auto desiredVelocity = target - from;
	desiredVelocity.normalise();
	desiredVelocity *= speed;
	return desiredVelocity - currentVelocity;
}

btVector3 MovableObject::Seek(const btVector3& from, const btVector3& target,
                              const float& speed, const btVector3& currentVelocity)
{
	auto desiredVelocity = target - from;
	desiredVelocity.normalize();
	desiredVelocity *= speed;
	return desiredVelocity - currentVelocity;
}

bool MovableObject::HasSeekTarget() const
{
	return mSeekTarget != nullptr;
}

bool MovableObject::CanSeekTarget(float minDistance) const
{
	if (mSeekTarget == nullptr) return false;
	if (mDirection != Ogre::Vector3::ZERO) return false;
	auto distance = (mSeekTarget->getPosition() - getPosition()).length();
	return distance > minDistance;
}

void MovableObject::SetSeekTarget(Ogre::SceneNode* target)
{
	if (target == this) return;
	mSeekTarget = target;
}

// Probably should move this out of this class as per single responsibility principle
void MovableObject::SeekTarget(Graph* graph, PathFinding& pathFinder)
{
	Ogre::Vector3 location = mSeekTarget->getPosition();
	int startNode;
	int goalNode;

	if (!mPath.empty() && location != mPath.back())
	{
		mPath.clear();
	}

	startNode = graph->getNode(getPosition());
	goalNode = graph->getNode(location);

	// check that goal node is not the same as start node
	if (goalNode != startNode)
	{
		// try to find path from start to goal node
		std::deque<int> path;

		// if path exists
		if (pathFinder.AStar(startNode, goalNode, *graph, path))
		{
			path.pop_front();
			path.pop_back();

			for (auto p : path)
			{
				auto pos = graph->getPosition(p);
				pos.y = 1.0f;
				AddNode(pos);
			}

			BuildPathVisualizer();
		}
	}
}

void MovableObject::Update(const float& minDistance, const float& deltaTime)
{
	if (mDirection == Ogre::Vector3::ZERO)
	{
		if (nextLocation())
		{
			mAnimationState = mEntity->getAnimationState("Walk");
			mAnimationState->setLoop(true);
			mAnimationState->setEnabled(true);
		}
	}
	else
	{
		mDistance = (mDestination - Convert(mRigidBody->getCenterOfMassPosition())).length();

		if (mDistance <= minDistance)
		{
			mDirection = Ogre::Vector3::ZERO;
			if (!nextLocation())
			{
				mRigidBody->clearForces();
				mRigidBody->setLinearVelocity(mRigidBody->getLinearVelocity() * 0.33f);
				mPathVisualizer->clear();
				mAnimationState = mEntity->getAnimationState("Idle");
				mAnimationState->setLoop(true);
				mAnimationState->setEnabled(true);
			}
			else
			{
				auto quat = LookRotation(mDirection, Ogre::Vector3::UNIT_Y);
				btTransform trans = mRigidBody->getCenterOfMassTransform();
				trans.setRotation(Convert(quat));
				mRigidBody->setCenterOfMassTransform(trans);
			}
		}
		else
		{
			mRigidBody->activate();
			mRigidBody->applyCentralForce(Seek(mRigidBody->getCenterOfMassPosition(),
			                                   Convert(mDestination),
			                                   mMoveSpeed,
			                                   mRigidBody->getLinearVelocity()));

			auto quat = LookRotation(mDirection, Ogre::Vector3::UNIT_Y);
			btTransform trans = mRigidBody->getCenterOfMassTransform();
			trans.setRotation(Convert(quat));
			mRigidBody->setCenterOfMassTransform(trans);
		}
	}

	if (mAnimationState)
		mAnimationState->addTime(deltaTime);
}

bool MovableObject::nextLocation()
{
	if (mPath.empty()) return false;
	BuildPathVisualizer();
	mDestination = mPath.front(); // this gets the front of the deque
	mPath.pop_front(); // this removes the front of the deque
	auto position = Convert(mRigidBody->getCenterOfMassPosition());
	mDestination.y = position.y;
	mDirection = mDestination - position;
	mDistance = mDirection.normalise();
	return true;
}

Ogre::ManualObject* MovableObject::GetPathVisualizer() const
{
	return mPathVisualizer;
}

void MovableObject::BuildPathVisualizer()
{
	mPathVisualizer->clear();

	// Specify the material and rendering type
	mPathVisualizer->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_STRIP);

	for (auto pos : mPath)
	{
		pos.y = 1.0f;
		mPathVisualizer->position(pos);
		mPathVisualizer->colour(mPathVizColour);
	}

	// Finished defining line
	mPathVisualizer->end();
}

void MovableObject::ClearPaths()
{
	mPath.clear();
}

void MovableObject::ShowPathVisualizer(bool show) const
{
	mPathVisualizer->getParentSceneNode()->setVisible(show);
}

void MovableObject::TogglePathVisualizer()
{
	mShowPathViz = !mShowPathViz;
	mPathVisualizer->getParentSceneNode()->setVisible(mShowPathViz);
}

void MovableObject::AttachDecal(Ogre::BillboardSet* billboardSet) const
{
	mDecal->attachObject(billboardSet);
	mDecal->setVisible(false);
}

void MovableObject::ShowDecal() const
{
	mDecal->setVisible(true);
}

void MovableObject::HideDecal() const
{
	mDecal->setVisible(false);
}
