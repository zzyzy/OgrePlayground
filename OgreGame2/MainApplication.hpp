//
// OgreGame2/MainApplication.hpp
// 
// Class that contains application specific members and
// methods. It inherits from OgreContext which implements
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

#include "OgreContext.hpp"
#include "BulletContext.hpp"
#include "FPSController.hpp"
#include "Weapon.hpp"

class MainApplication : public OgreContext
{
public:
	MainApplication();
	~MainApplication();

private:
	// FrameListener overrides
	bool frameRenderingQueued(const Ogre::FrameEvent& evt) override;

	// KeyListener overrides
	bool keyPressed(const OIS::KeyEvent& ke) override;
	bool keyReleased(const OIS::KeyEvent& ke) override;

	// MouseListener overrides
	bool mouseMoved(const OIS::MouseEvent& me) override;
	bool mousePressed(const OIS::MouseEvent& me, OIS::MouseButtonID id) override;
	bool mouseReleased(const OIS::MouseEvent& me, OIS::MouseButtonID id) override;

	void setupScene(Ogre::SceneManager* const sceneMgr) override;

	BulletContext mBulletContext;
	FPSController mFPSController;
	Weapon mWeapon;
	Ogre::Overlay* mSniperScopeOverlay;
	Ogre::Overlay* mBinocularOverlay;
};

#endif // #ifndef __MAINAPPLICATION_HPP__
