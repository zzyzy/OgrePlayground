//
// OgreGame3/MovableObject.hpp
// 
// TODO Add description
// TODO May need to change name to avoid conflict with Ogre::MovableObject
//
// Basic usage:
// 1. Use this instead of Ogre::SceneNode if you want a movable object (not Ogre::MovableObject)
//
// Author: Zhen Zhi Lee <leezhenzhi@gmail.com>
//
// Copyright (C) 2016 Zhen Zhi Lee.
//

#pragma once
#ifndef __MOVABLEOBJECT_HPP__
#define __MOVABLEOBJECT_HPP__

#include "PathFinding.h"

class MovableObject : public Ogre::SceneNode
{
public:
	MovableObject(Ogre::SceneManager* creator,
	              const Ogre::Real& moveSpeed,
	              const Ogre::ColourValue& pathColour,
	              Ogre::Entity* entity);

	// Needed to prevent some issue with calling virtual function in constructor
	void Setup(Ogre::SceneManager* creator);

	// Rigidbody
	void SetRigidBody(btRigidBody* rigidBody);
	btRigidBody* GetRigidBody() const;

	// Path
	void SetPath(const std::deque<Ogre::Vector3>& path);
	void AddNode(const Ogre::Vector3& location);
	bool IsPathEmpty() const;
	Ogre::Vector3 GetLastNode() const;

	// Seeking
	static Ogre::Vector3 Seek(const Ogre::Vector3& from, const Ogre::Vector3& target,
	                          const float& speed, const Ogre::Vector3& currentVelocity);
	static btVector3 Seek(const btVector3& from, const btVector3& target,
	                      const float& speed, const btVector3& currentVelocity);
	bool HasSeekTarget() const;
	bool CanSeekTarget(float minDistance) const;
	void SetSeekTarget(Ogre::SceneNode* target);
	// This should be called every frame
	void SeekTarget(Graph* graph, PathFinding& pathFinder);

	// This should be called every frame
	void Update(const float& minDistance, const float& deltaTime);

	// Path visualizer
	Ogre::ManualObject* GetPathVisualizer() const;
	void BuildPathVisualizer();
	void ClearPaths();
	void ShowPathVisualizer(bool show = true) const;
	void TogglePathVisualizer();

	// Decal
	void AttachDecal(Ogre::BillboardSet* billboardSet) const;
	void ShowDecal() const;
	void HideDecal() const;

protected:
	bool nextLocation();

	btRigidBody* mRigidBody;					// Attached rigidbody
	Ogre::ManualObject* mPathVisualizer;		// Path visualizer
	Ogre::ColourValue mPathVizColour;			// Path visualizer colour
	std::deque<Ogre::Vector3> mPath;			// Path to follow
	Ogre::Real mMoveSpeed;						// Movement speed
	Ogre::Real mDistance;						// Distance between current position and destination
	Ogre::Vector3 mDestination;					// Destination taken from the path
	Ogre::Vector3 mDirection;					// Direction towards the destination
	Ogre::SceneNode* mDecal;					// Attached decal
	Ogre::SceneNode* mSeekTarget;				// Target to seek
	Ogre::Entity* mEntity;						// Attached entity
	Ogre::AnimationState* mAnimationState;		// Animation state
	bool mShowPathViz;							// Whether to show path visualizer or not
};

#endif // __MOVABLEOBJECT_HPP__
