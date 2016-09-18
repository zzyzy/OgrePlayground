//
// OgreGame2/OgreContext.cpp
// 
// Author: Zhen Zhi Lee <leezhenzhi@gmail.com>
//
// Copyright (C) 2016 Zhen Zhi Lee.
//

#include "OgreContext.hpp"

OgreContext::OgreContext() :
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

OgreContext::~OgreContext()
{
	if (mTrayMgr) delete mTrayMgr;
	if (mOverlaySystem) delete mOverlaySystem;

    Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
    WindowEventListener::windowClosed(mWindow);
    delete mRoot;
}

bool OgreContext::Setup()
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

    auto ogreHome = getenv("OGRE_HOME");

    while (secIt.hasMoreElements())
    {
        Ogre::ConfigFile::SettingsMultiMap* settings = secIt.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator it;

        for (it = settings->begin(); it != settings->end(); ++it)
        {
            locType = it->first;
            name = ogreHome + it->second;

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

    //mCamera = mSceneMgr->createCamera("MainCam");
    //mCamera->setPosition(0, 0, 80);
    //mCamera->lookAt(0, 0, -300);
    //mCamera->setNearClipDistance(5);

    //Ogre::Viewport* vp = mWindow->addViewport(mCamera);
    //vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));

    //mCamera->setAspectRatio(
    //    Ogre::Real(vp->getActualWidth()) /
    //    Ogre::Real(vp->getActualHeight()));

    //// Create Scene
    //Ogre::Entity* ogreEntity = mSceneMgr->createEntity("ogrehead.mesh");

    //Ogre::SceneNode* ogreNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    //ogreNode->attachObject(ogreEntity);

    //mSceneMgr->setAmbientLight(Ogre::ColourValue(.5, .5, .5));

    //Ogre::Light* light = mSceneMgr->createLight("MainLight");
    //light->setPosition(20, 80, 50);

    setupCamera(mSceneMgr, mCamera);
    setupViewport(mWindow, mCamera);
    setupScene(mSceneMgr);

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
	mTrayMgr = new OgreBites::SdkTrayManager("InterfaceName", mWindow, mInputContext, this);
	mTrayMgr->showFrameStats(OgreBites::TL_BOTTOMLEFT);
	mTrayMgr->showLogo(OgreBites::TL_BOTTOMRIGHT);
	mTrayMgr->hideCursor();

    mRoot->addFrameListener(this);
    mMouse->setEventCallback(this);
    mKeyboard->setEventCallback(this);

    mRoot->startRendering();

    return true;
}

void OgreContext::windowResized(Ogre::RenderWindow* rw)
{
    int left, top;
    unsigned int width, height, depth;

    rw->getMetrics(width, height, depth, left, top);

    const OIS::MouseState& ms = mMouse->getMouseState();
    ms.width = width;
    ms.height = height;
}

void OgreContext::windowClosed(Ogre::RenderWindow* rw)
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

bool OgreContext::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    if (mWindow->isClosed()) return false;

    mKeyboard->capture();
    mMouse->capture();

    if (mKeyboard->isKeyDown(OIS::KC_ESCAPE)) return false;

	mTrayMgr->frameRenderingQueued(evt);

    return true;
}

bool OgreContext::mouseMoved(const OIS::MouseEvent& me)
{
	//if (!mTrayMgr->injectMouseMove(me)) return false;
	return true;
}

bool OgreContext::mousePressed(const OIS::MouseEvent& me, OIS::MouseButtonID id)
{
	//if (!mTrayMgr->injectMouseDown(me, id)) return false;
	return true;
}

bool OgreContext::mouseReleased(const OIS::MouseEvent& me, OIS::MouseButtonID id)
{
	//if (!mTrayMgr->injectMouseUp(me, id)) return false;
	return true;
}

void OgreContext::setupCamera(Ogre::SceneManager* const sceneMgr, Ogre::Camera*& camera) const
{
    camera = sceneMgr->createCamera("MainCam");
    camera->setPosition(0, 50, 80);
    camera->lookAt(0, 0, -300);
    camera->setNearClipDistance(5);
}

void OgreContext::setupViewport(Ogre::RenderWindow* const window, Ogre::Camera*& camera) const
{
    Ogre::Viewport* vp = window->addViewport(camera);
    vp->setBackgroundColour(Ogre::ColourValue(0.2f, 0.2f, 0.2f));

    camera->setAspectRatio(
        Ogre::Real(vp->getActualWidth()) /
        Ogre::Real(vp->getActualHeight())
    );
}
