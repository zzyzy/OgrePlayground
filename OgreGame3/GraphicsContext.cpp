//
// OgreGame3/GraphicsContext.cpp
// 
// Author: Zhen Zhi Lee <leezhenzhi@gmail.com>
//
// Copyright (C) 2016 Zhen Zhi Lee.
//

#include "GraphicsContext.hpp"

GraphicsContext::GraphicsContext() :
	mRoot(nullptr),
	mResourcesCfg(Ogre::StringUtil::BLANK),
	mPluginsCfg(Ogre::StringUtil::BLANK),
	mWindow(nullptr),
	mSceneMgr(nullptr),
	mCamera(nullptr),
	mOverlaySystem(nullptr),
	mInputMgr(nullptr),
	mKeyboard(nullptr),
	mMouse(nullptr),
	mTrayMgr(nullptr)
{
}

GraphicsContext::~GraphicsContext()
{
	Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
	WindowEventListener::windowClosed(mWindow);

	if (mTrayMgr) delete mTrayMgr;
	if (mOverlaySystem) delete mOverlaySystem;
	if (mRoot) delete mRoot;
}

bool GraphicsContext::Setup()
{
#ifdef _DEBUG
	mResourcesCfg = "resources_d.cfg";
	mPluginsCfg = "plugins_d.cfg";
#else
    mResourcesCfg = "resources.cfg";
    mPluginsCfg = "plugins.cfg";
#endif

	mRoot = new Ogre::Root(mPluginsCfg);

	Ogre::ConfigFile cf;
	cf.load(mResourcesCfg);

	Ogre::String name, locType;
	Ogre::ConfigFile::SectionIterator secIt = cf.getSectionIterator();

	while (secIt.hasMoreElements())
	{
		Ogre::ConfigFile::SettingsMultiMap* settings = secIt.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator it;

		for (it = settings->begin(); it != settings->end(); ++it)
		{
			locType = it->first;
			name = it->second;

			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(name, locType);
		}
	}

	if (!(mRoot->restoreConfig() || mRoot->showConfigDialog()))
		return false;

	mWindow = mRoot->initialise(true, "TutorialApplication Render Window");

	mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);

	mOverlaySystem = new Ogre::OverlaySystem();
	mSceneMgr->addRenderQueueListener(mOverlaySystem);

	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	SetupCamera(mSceneMgr, mCamera);
	SetupViewport(mWindow, mCamera);
	SetupScene(mSceneMgr);

	// OIS
	Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");

	OIS::ParamList pl;
	size_t windowHandle = 0;
	std::ostringstream windowHandleStr;

	mWindow->getCustomAttribute("WINDOW", &windowHandle);
	windowHandleStr << windowHandle;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHandleStr.str()));

	mInputMgr = OIS::InputManager::createInputSystem(pl);

	mKeyboard = static_cast<OIS::Keyboard*>(
		mInputMgr->createInputObject(OIS::OISKeyboard, true));
	mMouse = static_cast<OIS::Mouse*>(
		mInputMgr->createInputObject(OIS::OISMouse, true));

	windowResized(mWindow);
	Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);

	mInputContext.mKeyboard = mKeyboard;
	mInputContext.mMouse = mMouse;

	SetupTrayUI(mSceneMgr, mTrayMgr);

	mRoot->addFrameListener(this);
	mMouse->setEventCallback(this);
	mKeyboard->setEventCallback(this);

	mRoot->startRendering();

	return true;
}

OgreBites::SdkTrayManager* GraphicsContext::GetTrayMgr() const
{
	return mTrayMgr;
}

void GraphicsContext::windowResized(Ogre::RenderWindow* rw)
{
	int left, top;
	unsigned int width, height, depth;

	rw->getMetrics(width, height, depth, left, top);

	const OIS::MouseState& ms = mMouse->getMouseState();
	ms.width = width;
	ms.height = height;
}

void GraphicsContext::windowClosed(Ogre::RenderWindow* rw)
{
	if (rw == mWindow)
	{
		if (mInputMgr)
		{
			mInputMgr->destroyInputObject(mMouse);
			mInputMgr->destroyInputObject(mKeyboard);

			OIS::InputManager::destroyInputSystem(mInputMgr);
			mInputMgr = nullptr;
		}
	}
}

bool GraphicsContext::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	if (mWindow->isClosed()) return false;

	mKeyboard->capture();
	mMouse->capture();

	if (mKeyboard->isKeyDown(OIS::KC_ESCAPE)) return false;

	mTrayMgr->frameRenderingQueued(evt);

	return true;
}

bool GraphicsContext::mouseMoved(const OIS::MouseEvent& me)
{
	mTrayMgr->injectMouseMove(me);
	return true;
}

bool GraphicsContext::mousePressed(const OIS::MouseEvent& me, OIS::MouseButtonID id)
{
	mTrayMgr->injectMouseDown(me, id);
	return true;
}

bool GraphicsContext::mouseReleased(const OIS::MouseEvent& me, OIS::MouseButtonID id)
{
	mTrayMgr->injectMouseUp(me, id);
	return true;
}

void GraphicsContext::SetupCamera(Ogre::SceneManager* const sceneMgr, Ogre::Camera*& camera)
{
	camera = sceneMgr->createCamera("MainCamera");
	camera->setNearClipDistance(0.1f);
}

void GraphicsContext::SetupViewport(Ogre::RenderWindow* const window, Ogre::Camera*& camera) const
{
	Ogre::Viewport* vp = window->addViewport(camera);
	vp->setBackgroundColour(Ogre::ColourValue(0.2f, 0.2f, 0.2f));

	camera->setAspectRatio(
		Ogre::Real(vp->getActualWidth()) /
		Ogre::Real(vp->getActualHeight())
	);
}

void GraphicsContext::SetupTrayUI(Ogre::SceneManager* const sceneMgr, OgreBites::SdkTrayManager*& trayMgr)
{
	trayMgr = new OgreBites::SdkTrayManager("InterfaceName", mWindow, mInputContext, this);
	trayMgr->showFrameStats(OgreBites::TL_BOTTOMLEFT);
	//trayMgr->showLogo(OgreBites::TL_BOTTOMRIGHT);
	//trayMgr->hideCursor();
}
