#pragma once

class IApplicationBehaviour
{
public:
	virtual void SetupCamera(Ogre::SceneManager* const sceneMgr, Ogre::Camera* camera) = 0;
	virtual void SetupViewport(Ogre::RenderWindow* const window, Ogre::Camera* camera) = 0;
	virtual void SetupTrayUI(Ogre::SceneManager* const sceneMgr, OgreBites::SdkTrayManager* trayMgr) = 0;
	virtual void SetupScene(Ogre::SceneManager* const sceneMgr, Ogre::Camera* camera) = 0;
};
