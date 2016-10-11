//
// OgreGame3/GraphicsContext.hpp
// 
// This class contains all the common Ogre operations and
// behaviours. It should be subclassed by another class for
// application specific operations. This class does the following
// in order:
//
// 1.  Create an Ogre root object
// 2.  Load resources
// 3.  Setup Ogre config
// 4.  Create a render window
// 5.  Initialize loaded resources
// 6.  Create a scene manager
// 7.  Setup camera, viewport
// 8.  Setup scene
// 9.  Setup OIS
// 10. Setup event listeners
// 11. Start rendering
//
// Most if not all code are taken from the basic tutorials of
// http://www.ogre3d.org/tikiwiki/. Many thanks to the wiki team.
//
// Author: Zhen Zhi Lee <leezhenzhi@gmail.com>
//
// Copyright (C) 2016 Zhen Zhi Lee.
//

#pragma once
#ifndef __GRAPHICSCONTEXT_HPP__
#define __GRAPHICSCONTEXT_HPP__

#include "IApplicationBehaviour.hpp"

class GraphicsContext : public Ogre::WindowEventListener
{
public:
	GraphicsContext();
	~GraphicsContext();

	bool Setup(Ogre::FrameListener* frameListener,
	           OIS::MouseListener* mouseListener,
	           OIS::KeyListener* keyListener,
	           OgreBites::SdkTrayListener* sdkTrayListener,
	           IApplicationBehaviour* appBehaviour);

	OgreBites::SdkTrayManager* GetTrayMgr() const;

	// FrameListener
	bool CaptureRenderQueue(const Ogre::FrameEvent& evt) const;

	// MouseListener
	bool CaptureMouseMoved(const OIS::MouseEvent& me) const;
	bool CaptureMousePressed(const OIS::MouseEvent& me, const OIS::MouseButtonID& id) const;
	bool CaptureMouseReleased(const OIS::MouseEvent& me, const OIS::MouseButtonID& id) const;

private:
	// WindowEventListener overrides
	void windowResized(Ogre::RenderWindow* rw) final override;
	void windowClosed(Ogre::RenderWindow* rw) final override;

	// Part of the setup sequence
	void setupCamera();
	void setupViewport();
	void setupTrayUI();
	void setupScene();

	// Ogre
	Ogre::Root* mRoot;
	Ogre::String mResourcesCfg;
	Ogre::String mPluginsCfg;
	Ogre::RenderWindow* mWindow;
	Ogre::SceneManager* mSceneMgr;
	Ogre::Camera* mCamera;
	Ogre::OverlaySystem* mOverlaySystem;

	// OIS
	OIS::InputManager* mInputMgr;
	OIS::Keyboard* mKeyboard;
	OIS::Mouse* mMouse;

	// OgreBites
	OgreBites::InputContext mInputContext;
	OgreBites::SdkTrayManager* mTrayMgr;

	// Overrides (application specific behaviours)
	IApplicationBehaviour* mAppBehaviour;
};

#endif // #ifndef __GRAPHICSCONTEXT_HPP__
