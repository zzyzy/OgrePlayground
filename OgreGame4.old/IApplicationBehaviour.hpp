//
// OgreGame3/IApplicationBehaviour.hpp
// 
// TODO Add description
//
// Author: Zhen Zhi Lee <leezhenzhi@gmail.com>
//
// Copyright (C) 2016 Zhen Zhi Lee.
//

#pragma once
#ifndef __IAPPLICATIONBEHAVIOUR_HPP__
#define __IAPPLICATIONBEHAVIOUR_HPP__

class IApplicationBehaviour
{
public:
	virtual ~IApplicationBehaviour()
	{
	}

	virtual void SetupCamera(Ogre::SceneManager* const sceneMgr, Ogre::Camera* camera) = 0;
	virtual void SetupViewport(Ogre::RenderWindow* const window, Ogre::Camera* camera) = 0;
	virtual void SetupTrayUI(Ogre::SceneManager* const sceneMgr, OgreBites::SdkTrayManager* trayMgr) = 0;
	virtual void SetupScene(Ogre::SceneManager* const sceneMgr, Ogre::Camera* camera) = 0;
};

#endif // __IAPPLICATIONBEHAVIOUR_HPP__