//
// OgreGame2/OgreContext.hpp
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
#ifndef __OGRECONTEXT_HPP__
#define __OGRECONTEXT_HPP__

#include <OgreRoot.h>
#include <OgreWindowEventUtilities.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

class OgreContext :
    public Ogre::WindowEventListener,
    public Ogre::FrameListener,
    public OIS::KeyListener,
    public OIS::MouseListener,
	public OgreBites::SdkTrayListener
{
public:
    OgreContext();
    virtual ~OgreContext();

    bool Setup();

protected:
    // WindowEventListener overrides
    void windowResized(Ogre::RenderWindow* rw) final override;
    void windowClosed(Ogre::RenderWindow* rw) final override;

    // FrameListener overrides
    bool frameRenderingQueued(const Ogre::FrameEvent& evt) override;

	// MouseListener overrides
	bool mouseMoved(const OIS::MouseEvent& me) override;
	bool mousePressed(const OIS::MouseEvent& me, OIS::MouseButtonID id) override;
	bool mouseReleased(const OIS::MouseEvent& me, OIS::MouseButtonID id) override;

    // Allow application specific overrides
    virtual void setupCamera(Ogre::SceneManager* const sceneMgr, Ogre::Camera*& camera) const;
    virtual void setupViewport(Ogre::RenderWindow* const window, Ogre::Camera*& camera) const;
    virtual void setupScene(Ogre::SceneManager* const sceneMgr) = 0;
	virtual void setupUI(Ogre::SceneManager* const sceneMgr) = 0;

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
};

#endif // #ifndef __OGRECONTEXT_HPP__
