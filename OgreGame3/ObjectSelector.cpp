//
// OgreGame3/ObjectSelector.cpp
//
// Author: Zhen Zhi Lee <leezhenzhi@gmail.com>
//
// Copyright (C) 2016 Zhen Zhi Lee.
//

#include "ObjectSelector.hpp"

SelectionBox::SelectionBox(const Ogre::String& name) :
	ManualObject(name)
{
	setup();
}

void SelectionBox::setup()
{
	setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY);
	setUseIdentityProjection(true);
	setUseIdentityView(true);
	setQueryFlags(0);
}

SelectionBox::~SelectionBox()
{
}

void SelectionBox::SetCorners(float left, float top, float right, float bottom)
{
	left = 2 * left - 1;
	right = 2 * right - 1;
	top = 1 - 2 * top;
	bottom = 1 - 2 * bottom;

	clear();
	setBoundingBox(Ogre::AxisAlignedBox::BOX_INFINITE);
	begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_STRIP);
	colour(Ogre::ColourValue::Green);
	position(left, top, -1);
	position(right, top, -1);
	position(right, bottom, -1);
	position(left, bottom, -1);
	position(left, top, -1);
	end();
}

void SelectionBox::SetCorners(const Ogre::Vector2& topLeft, const Ogre::Vector2& bottomRight)
{
	SetCorners(topLeft.x, topLeft.y, bottomRight.x, bottomRight.y);
}

ObjectSelector::ObjectSelector(Ogre::SceneManager* sceneMgr, Ogre::Camera* camera) :
	mSceneMgr(sceneMgr),
	mCamera(camera),
	mSelectionBox(nullptr),
	mSelecting(false),
	mModifySelection(false)
{
	mSelectionBox = OGRE_NEW SelectionBox("SelectionBox");
	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(mSelectionBox);
}

ObjectSelector::ObjectSelector() :
	mSceneMgr(nullptr),
	mCamera(nullptr),
	mSelectionBox(nullptr),
	mSelecting(false),
	mModifySelection(false)
{
}

ObjectSelector::ObjectSelector(const ObjectSelector& objectSelector) :
	mSceneMgr(objectSelector.mSceneMgr),
	mCamera(objectSelector.mCamera),
	mSelectionBox(objectSelector.mSelectionBox),
	mSelecting(objectSelector.mSelecting),
	mModifySelection(objectSelector.mModifySelection)
{
}

ObjectSelector::~ObjectSelector()
{
}

void ObjectSelector::Setup(Ogre::SceneManager* sceneMgr, Ogre::Camera* camera)
{
	mSceneMgr = sceneMgr;
	mCamera = camera;
	mSelectionBox = OGRE_NEW SelectionBox("SelectionBox");
	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(mSelectionBox);
}

bool ObjectSelector::CaptureKeyPressed(const OIS::KeyEvent& ke)
{
	switch (ke.key)
	{
	case OIS::KC_LSHIFT:
		mModifySelection = true;
		break;
	default:
		break;
	}

	return true;
}

bool ObjectSelector::CaptureKeyReleased(const OIS::KeyEvent& ke)
{
	switch (ke.key)
	{
	case OIS::KC_LSHIFT:
		mModifySelection = false;
		break;
	default:
		break;
	}

	return true;
}

bool ObjectSelector::CaptureMouseMoved(const OIS::MouseEvent& me)
{
	if (mSelecting)
	{
		mEndLoc.x = me.state.X.abs / float(me.state.width);
		mEndLoc.y = me.state.Y.abs / float(me.state.height);
		mSelectionBox->SetCorners(mStartLoc, mEndLoc);
	}

	return true;
}

bool ObjectSelector::CaptureMousePressed(const OIS::MouseEvent& me, const OIS::MouseButtonID& id)
{
	switch (id)
	{
	case OIS::MB_Left:
		mStartLoc.x = me.state.X.abs / float(me.state.width);
		mStartLoc.y = me.state.Y.abs / float(me.state.height);
		mEndLoc = mStartLoc;
		mSelecting = true;
		mSelectionBox->clear();
		mSelectionBox->setVisible(true);
		break;
	default:
		break;
	}

	return true;
}

bool ObjectSelector::CaptureMouseReleased(const OIS::MouseEvent& me, const OIS::MouseButtonID& id)
{
	switch (id)
	{
	case OIS::MB_Left:
		// In WC3, they allow extremely small selection box to select a single object
		// However, I have failed to duplicate that, may need to look into it next time
		if (IsSelectionBoxLargeEnough(mStartLoc, mEndLoc))
		{
			SelectObjects(mStartLoc, mEndLoc, mModifySelection, mSelectedObjects);
		}
		else
		{
			SelectObject(me, mModifySelection, mSelectedObjects);
		}

		mSelecting = false;
		mSelectionBox->setVisible(false);
		break;
	default:
		break;
	}

	return true;
}

bool ObjectSelector::IsSelectionBoxLargeEnough(const Ogre::Vector2& first, const Ogre::Vector2& second) const
{
	float left = first.x, right = second.x;
	float top = first.y, bottom = second.y;

	if (left > right)
	{
		std::swap(left, right);
	}

	if (top > bottom)
	{
		std::swap(top, bottom);
	}

	if ((right - left) * (bottom - top) < 0.0001f)
	{
		return false;
	}

	return true;
}

void ObjectSelector::SelectObject(const OIS::MouseEvent& me, const bool& modifySelection,
                                  std::set<MovableObject*>& selectedObjects) const
{
	Ogre::Ray mouseRay = mCamera->getCameraToViewportRay(
		static_cast<float>(me.state.X.abs) / me.state.width,
		static_cast<float>(me.state.Y.abs) / me.state.height);

	auto raySceneQuery = mSceneMgr->createRayQuery(Ogre::Ray());
	raySceneQuery->setRay(mouseRay);
	raySceneQuery->setSortByDistance(true);
	raySceneQuery->setQueryMask(ROBOT_MASK);

	Ogre::RaySceneQueryResult& result = raySceneQuery->execute();
	Ogre::RaySceneQueryResult::iterator itr = result.begin();

	if (!modifySelection)
	{
		for (auto it = selectedObjects.begin(); it != selectedObjects.end(); ++it)
		{
			//(*it)->showBoundingBox(false);
			(*it)->HideDecal();
		}
		selectedObjects.clear();
	}

	if (itr != result.end())
	{
		if (itr->movable)
		{
			// Entity is attached to a child node of the MovableObject (not Ogre's) node
			auto selectedObject = static_cast<MovableObject*>(itr->movable->getParentSceneNode()->getParentSceneNode());

			if (modifySelection && selectedObjects.find(selectedObject) != selectedObjects.end())
			{
				//selectedObject->showBoundingBox(false);
				selectedObject->HideDecal();
				selectedObjects.erase(selectedObject);
			}
			else
			{
				//selectedObject->showBoundingBox(true);
				selectedObject->ShowDecal();
				selectedObjects.insert(selectedObject);
			}
		}
	}

	mSceneMgr->destroyQuery(raySceneQuery);
}

void ObjectSelector::SelectObjects(const Ogre::Vector2& first, const Ogre::Vector2& second, const bool& modifySelection,
                                   std::set<MovableObject*>& selectedObjects) const
{
	auto volQuery = mSceneMgr->createPlaneBoundedVolumeQuery(Ogre::PlaneBoundedVolumeList());
	float left = first.x, right = second.x;
	float top = first.y, bottom = second.y;
	volQuery->setQueryMask(ROBOT_MASK);

	if (left > right)
	{
		std::swap(left, right);
	}

	if (top > bottom)
	{
		std::swap(top, bottom);
	}

	if ((right - left) * (bottom - top) < 0.0001f)
		return;

	Ogre::Ray topLeft = mCamera->getCameraToViewportRay(left, top);
	Ogre::Ray topRight = mCamera->getCameraToViewportRay(right, top);
	Ogre::Ray bottomLeft = mCamera->getCameraToViewportRay(left, bottom);
	Ogre::Ray bottomRight = mCamera->getCameraToViewportRay(right, bottom);

	Ogre::Plane frontPlane, topPlane, leftPlane, bottomPlane, rightPlane;

	frontPlane = Ogre::Plane(
		topLeft.getOrigin(),
		topRight.getOrigin(),
		bottomRight.getOrigin());

	topPlane = Ogre::Plane(
		topLeft.getOrigin(),
		topLeft.getPoint(10),
		topRight.getPoint(10));

	leftPlane = Ogre::Plane(
		topLeft.getOrigin(),
		bottomLeft.getPoint(10),
		topLeft.getPoint(10));

	bottomPlane = Ogre::Plane(
		bottomLeft.getOrigin(),
		bottomRight.getPoint(10),
		bottomLeft.getPoint(10));

	rightPlane = Ogre::Plane(
		topRight.getOrigin(),
		topRight.getPoint(10),
		bottomRight.getPoint(10));

	Ogre::PlaneBoundedVolume vol;

	vol.planes.push_back(frontPlane);
	vol.planes.push_back(topPlane);
	vol.planes.push_back(leftPlane);
	vol.planes.push_back(bottomPlane);
	vol.planes.push_back(rightPlane);

	Ogre::PlaneBoundedVolumeList volList;
	volList.push_back(vol);

	volQuery->setVolumes(volList);
	Ogre::SceneQueryResult result = volQuery->execute();

	if (!modifySelection)
	{
		for (auto it = selectedObjects.begin(); it != selectedObjects.end(); ++it)
		{
			//(*it)->showBoundingBox(false);
			(*it)->HideDecal();
		}
		selectedObjects.clear();
	}

	for (auto it = result.movables.begin(); it != result.movables.end(); ++it)
	{
		auto selectedObject = static_cast<MovableObject*>((*it)->getParentSceneNode()->getParentSceneNode());
		//selectedObject->showBoundingBox(true);
		selectedObject->ShowDecal();
		selectedObjects.insert(selectedObject);
	}

	mSceneMgr->destroyQuery(volQuery);
}

bool ObjectSelector::HasSelection() const
{
	return mSelectedObjects.empty() != true;
}

std::set<MovableObject*> ObjectSelector::GetSelections() const
{
	return mSelectedObjects;
}
