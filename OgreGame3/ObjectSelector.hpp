//
// OgreGame3/ObjectSelector.hpp
// 
// TODO Add description
//
// Basic usage:
// 1. Add it to a class as member
// 2. Setup the appropriate event listeners
//
// Author: Zhen Zhi Lee <leezhenzhi@gmail.com>
//
// Copyright (C) 2016 Zhen Zhi Lee.
//

#pragma once
#ifndef __OBJECTSELECTOR_HPP__
#define __OBJECTSELECTOR_HPP__

#include "MovableObject.hpp"

// Probably should inject this into ObjectSelector instead of hardcoding it
enum QueryFlags
{
	ENVIRONMENT_MASK = 1 << 0,
	ROBOT_MASK = 1 << 1
};

class SelectionBox : public Ogre::ManualObject
{
public:
	explicit SelectionBox(const Ogre::String& name);
	virtual ~SelectionBox();

	void SetCorners(float left, float top, float right, float bottom);
	void SetCorners(const Ogre::Vector2& topLeft, const Ogre::Vector2& bottomRight);

private:
	void setup();
};

class ObjectSelector
{
public:
	ObjectSelector(Ogre::SceneManager* sceneMgr, Ogre::Camera* camera);
	ObjectSelector();
	~ObjectSelector();

	// Key events
	bool CaptureKeyPressed(const OIS::KeyEvent& ke);
	bool CaptureKeyReleased(const OIS::KeyEvent& ke);

	// Mouse events
	bool CaptureMouseMoved(const OIS::MouseEvent& me);
	bool CaptureMousePressed(const OIS::MouseEvent& me, const OIS::MouseButtonID& id);
	bool CaptureMouseReleased(const OIS::MouseEvent& me, const OIS::MouseButtonID& id);

	// These should be used internally but nevertheless exposing it so that 
	// we can customize it some day (by not using the callbacks provided)
	bool IsSelectionBoxLargeEnough(const Ogre::Vector2& first, const Ogre::Vector2& second);
	void SelectObject(const OIS::MouseEvent& me, const bool& modifySelection,
		std::set<MovableObject*>& selectedObjects);
	void SelectObjects(const Ogre::Vector2& first, const Ogre::Vector2& second,
		const bool& modifySelection, std::set<MovableObject*>& selectedObjects);

	// These can be used by other objects/classes
	bool HasSelection() const;
	std::set<MovableObject*> GetSelections() const;

private:
	Ogre::SceneManager* mSceneMgr;							// Scene manager needed to create the ray query
	Ogre::Camera* mCamera;									// ^ needed for ray query too
	SelectionBox* mSelectionBox;							// Selection box
	bool mSelecting;										// If it is selecting multiple objects
	bool mModifySelection;									// Whether we want to add/remove selections
	Ogre::Vector2 mStartLoc;								// Selection box top left x y coords
	Ogre::Vector2 mEndLoc;									// Selection box bottom right x y coords
	std::set<MovableObject*> mSelectedObjects;				// Set of selected objects
	Ogre::RaySceneQuery* mRaySceneQuery;					// Raycasting scene query
	Ogre::PlaneBoundedVolumeListSceneQuery* mVolQuery;		// Volumetric scene query
};

#endif // __OBJECTSELECTOR_HPP__
