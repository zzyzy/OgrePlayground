//
// OgreGame3/MainApplication.hpp
// 
// Class that contains application specific members and
// methods. It inherits from GraphicsContext which implements
// most of the Ogre specific operations but are common to
// all applications using Ogre.
//
// Author: Zhen Zhi Lee <leezhenzhi@gmail.com>
//
// Copyright (C) 2016 Zhen Zhi Lee.
//

#pragma once
#ifndef __MAINAPPLICATION_HPP__
#define __MAINAPPLICATION_HPP__

#include "GraphicsContext.hpp"
#include "PhysicsContext.hpp"
#include "RTSController.hpp"
#include "ObjectSelector.hpp"

class MainApplication :
	public Ogre::FrameListener,
	public OIS::KeyListener,
	public OIS::MouseListener,
	public OgreBites::SdkTrayListener,
	public IApplicationBehaviour
{
public:
	MainApplication();
	~MainApplication();

private:
	// FrameListener
	bool frameRenderingQueued(const Ogre::FrameEvent& evt) override;

	// KeyListener
	bool keyPressed(const OIS::KeyEvent& ke) override;
	bool keyReleased(const OIS::KeyEvent& ke) override;

	// MouseListener
	bool mouseMoved(const OIS::MouseEvent& me) override;
	bool mousePressed(const OIS::MouseEvent& me, OIS::MouseButtonID id) override;
	bool mouseReleased(const OIS::MouseEvent& me, OIS::MouseButtonID id) override;

	// Application specific behaviours
	void SetupCamera(Ogre::SceneManager* const sceneMgr, Ogre::Camera* camera) override;
	void SetupViewport(Ogre::RenderWindow* const window, Ogre::Camera* camera) override;
	void SetupTrayUI(Ogre::SceneManager* const sceneMgr, OgreBites::SdkTrayManager* trayMgr) override;
	void SetupScene(Ogre::SceneManager* const sceneMgr) override;

	GraphicsContext mGraphicsContext;
	PhysicsContext mPhysicsContext;
	RTSController mRTSController;
	ObjectSelector mObjectSelector;
};

#endif // #ifndef __MAINAPPLICATION_HPP__
