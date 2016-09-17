//
//  OgreBasicTut/MainApplication.hpp
//
//  Main application class that holds and manages the state of the application
// 
//  Author: Zhen Zhi Lee <leezhenzhi@gmail.com>
//
//  Copyright (C) 2016 Zhen Zhi Lee.
//
//	This file contains adapted codes from the basic tutorials of
//  http://www.ogre3d.org/tikiwiki/. Many thanks to the wiki team.
//

#pragma once
#ifndef __MAINAPPLICATION_HPP__
#define __MAINAPPLICATION_HPP__

#include "CameraController.hpp"

class MainApplication : 
	public Ogre::WindowEventListener, 
	public Ogre::FrameListener, 
	public OIS::KeyListener,
	public OIS::MouseListener, 
	public OgreBites::SdkTrayListener
{
public:
	MainApplication();
	~MainApplication();

	void	Run();
	void	SetOgreHome(const std::string &ogreHomePath);

protected:
	// WindowEventListener overrides
	void	windowResized(Ogre::RenderWindow *rw)	override;
	void	windowClosed(Ogre::RenderWindow *rw)	override;

	// KeyListener overrides
	bool	keyPressed(const OIS::KeyEvent &ke)		override;
	bool	keyReleased(const OIS::KeyEvent &ke)	override;

	// MouseListener overrides
	bool	mouseMoved(const OIS::MouseEvent &me)								override;
	bool	mousePressed(const OIS::MouseEvent &me, OIS::MouseButtonID id)		override;
	bool	mouseReleased(const OIS::MouseEvent &me, OIS::MouseButtonID id)		override;
	
	// FrameListener overrides
	bool	frameRenderingQueued(const Ogre::FrameEvent &evt)	override;

private:
	/*
	void			createRoot();
	void			defineResources()		const;
	void			setupRenderSystem()		const;
	void			createWindow();
	static void		initResources();
	void			createSceneMgr();
	*/
	void			createCamera();
	void			createViewports()		const;
	void			createScene();
	void			setupOIS();
	void			setupListeners();

	// Ogre
	Ogre::String				_ogreHomePath;
	Ogre::Root					*_root;
	Ogre::String				_resourcesCfg;
	Ogre::String				_pluginsCfg;
	Ogre::RenderWindow			*_window;
	Ogre::SceneManager			*_sceneMgr;
	Ogre::Camera				*_camera;
	Ogre::OverlaySystem			*mOverlaySystem;

	// OIS
	OIS::InputManager			*_inputMgr;
	OIS::Mouse					*_mouse;
	OIS::Keyboard				*_keyboard;

	// OgreBites
	OgreBites::InputContext		mInputContext;
	OgreBites::SdkTrayManager	*mTrayMgr;

	// Application
	CameraController			mCamController;
	bool						_quit;
};

#endif // #ifndef __MAINAPPLICATION_HPP__
