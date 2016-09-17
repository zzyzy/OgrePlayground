//
//  OgreBasicTut/MainApplication.cpp
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

#include "MainApplication.hpp"
#include "Constants.hpp"

MainApplication::MainApplication() : 
	_ogreHomePath(Ogre::StringUtil::BLANK),
	_root(nullptr),
	_resourcesCfg(Ogre::StringUtil::BLANK),
	_pluginsCfg(Ogre::StringUtil::BLANK),
	_window(nullptr),
	_sceneMgr(nullptr),
	_camera(nullptr),
	mOverlaySystem(nullptr),
	_inputMgr(nullptr),
	_mouse(nullptr),
	_keyboard(nullptr),
	mTrayMgr(nullptr),
	_quit(false)
{

}

MainApplication::~MainApplication()
{
	// TODO Something wrong here
	//if (mOverlaySystem) delete mOverlaySystem;
	if (mTrayMgr) delete mTrayMgr;

	Ogre::WindowEventUtilities::removeWindowEventListener(_window, this);
	MainApplication::windowClosed(_window);
	delete _root;
}

void MainApplication::Run()
{
	// Create root
#ifdef _DEBUG
	_resourcesCfg = k::RESOURCES_D_FILE;
	_pluginsCfg = k::PLUGINS_D_FILE;
#else
	_resourcesCfg = k::RESOURCES_FILE;
	_pluginsCfg = k::PLUGINS_FILE;
#endif

	_root = new Ogre::Root(_pluginsCfg);

	// Define resources
	Ogre::ConfigFile cf;
	cf.load(_resourcesCfg);

	Ogre::String name, locType;
	auto secIt = cf.getSectionIterator();
	while (secIt.hasMoreElements())
	{
		auto *settings = secIt.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator it;

		for (it = settings->begin(); it != settings->end(); ++it)
		{
			locType = it->first;
			name = _ogreHomePath + it->second;

			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(name, locType);
		}
	}

	// Setup render system
	if (!(_root->restoreConfig() || _root->showConfigDialog()))
		return;

	_window = _root->initialise(true, k::WINDOW_TITLE);

	// Setup scene manager
	_sceneMgr = _root->createSceneManager(Ogre::ST_GENERIC);
	mOverlaySystem = new Ogre::OverlaySystem();
	_sceneMgr->addRenderQueueListener(mOverlaySystem);

	createCamera();
	createViewports();

	// Set texture midmap level and load resources
	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(k::NUM_MIDMAP);
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	createScene();
	setupOIS();
	setupListeners();

	// Setup OgreBites SdkTray
	mInputContext.mKeyboard = _keyboard;
	mInputContext.mMouse = _mouse;
	mTrayMgr = new OgreBites::SdkTrayManager("InterfaceName", _window, mInputContext, this);
	mTrayMgr->showFrameStats(OgreBites::TL_BOTTOMLEFT);
	mTrayMgr->showLogo(OgreBites::TL_BOTTOMRIGHT);
	mTrayMgr->hideCursor();

	_root->startRendering();
}

void MainApplication::SetOgreHome(const std::string &ogreHomePath)
{
	_ogreHomePath = ogreHomePath;
}

void MainApplication::windowResized(Ogre::RenderWindow *rw)
{
	unsigned int width, height, depth;
	int left, top;
	rw->getMetrics(width, height, depth, left, top);

	const auto &ms = _mouse->getMouseState();
	ms.width = width;
	ms.height = height;
}

void MainApplication::windowClosed(Ogre::RenderWindow *rw)
{
	//Only close for window that created OIS (the main window in these demos)
	if (rw == _window)
	{
		if (_inputMgr)
		{
			_inputMgr->destroyInputObject(_mouse);
			_inputMgr->destroyInputObject(_keyboard);

			OIS::InputManager::destroyInputSystem(_inputMgr);
			_inputMgr = nullptr;
		}
	}
}

bool MainApplication::keyPressed(const OIS::KeyEvent& ke)
{
	switch (ke.key)
	{
	case OIS::KC_ESCAPE:
		_quit = true;
		break;
	case OIS::KC_SYSRQ:
		_window->writeContentsToTimestampedFile("screenshot", ".jpg");
		break;
	case OIS::KC_F11:
	{
		auto opti = _root->getRenderSystem()->getConfigOptions().find("Video Mode");
		auto vmopts = Ogre::StringUtil::split(opti->second.currentValue, " x");
		auto w = Ogre::StringConverter::parseUnsignedInt(vmopts[0]);
		auto h = Ogre::StringConverter::parseUnsignedInt(vmopts[1]);
		_window->setFullscreen(!_window->isFullScreen(), w, h);
		break;
	}
	case OIS::KC_Z:
	{
		auto *parentNode = _camera->getParentSceneNode();
		auto *cameraNode = _sceneMgr->getSceneNode(k::CAMERA_NODE);
		parentNode->detachObject(_camera);
		cameraNode->attachObject(_camera);
		mCamController.SetStyle(CameraStyle::FREE_ROAM);
		mCamController.SetObject(cameraNode);
		break;
	}
	case OIS::KC_X:
	{
		auto *parentNode = _camera->getParentSceneNode();
		auto *cameraNode = _sceneMgr->getSceneNode(k::CAMERA_NODE);
		parentNode->detachObject(_camera);
		cameraNode->attachObject(_camera);
		mCamController.SetStyle(CameraStyle::ORBIT);
		mCamController.SetObject(cameraNode);
		mCamController.SetTarget(_sceneMgr->getRootSceneNode());
		break;
	}
	case OIS::KC_C:
	{
		auto *parentNode = _camera->getParentSceneNode();
		auto *robotNode = _sceneMgr->getSceneNode(k::ROBOT_NODE);
		auto *cameraNode = _sceneMgr->getSceneNode(k::ROBOT_CAMERA_NODE);
		parentNode->detachObject(_camera);
		cameraNode->attachObject(_camera);
		mCamController.SetStyle(CameraStyle::THIRD_PERSON);
		mCamController.SetObject(robotNode);
		mCamController.SetCamera(cameraNode);
		break;
	}
	case OIS::KC_H:
	{
		if (mTrayMgr->isLogoVisible())
		{
			mTrayMgr->hideLogo();
		}
		else
		{
			mTrayMgr->showLogo(OgreBites::TL_BOTTOMRIGHT);
		}

		if (mTrayMgr->areFrameStatsVisible()) 
		{
			mTrayMgr->hideFrameStats();
		}
		else
		{
			mTrayMgr->showFrameStats(OgreBites::TL_BOTTOMLEFT);
		}
		break;
	}
	default:
		break;
	}

	mCamController.InjectKeyPressed(ke);

	return true;
}

bool MainApplication::keyReleased(const OIS::KeyEvent& ke)
{
	mCamController.InjectKeyReleased(ke);
	return true;
}

bool MainApplication::mouseMoved(const OIS::MouseEvent& me)
{
	if (mTrayMgr->injectMouseMove(me)) return true;
	mCamController.InjectMouseMoved(me);
	return true;
}

bool MainApplication::mousePressed(const OIS::MouseEvent& me, OIS::MouseButtonID id)
{
	if (mTrayMgr->injectMouseDown(me, id)) return true;
	mCamController.InjectMousePressed(me, id);
	return true;
}

bool MainApplication::mouseReleased(const OIS::MouseEvent& me, OIS::MouseButtonID id)
{
	if (mTrayMgr->injectMouseUp(me, id)) return true;
	mCamController.InjectMouseReleased(me, id);
	return true;
}

bool MainApplication::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	if (_window->isClosed())
		return false;

	if (_quit)
		return false;

	//Need to capture/update each device
	_keyboard->capture();
	_mouse->capture();

	mCamController.InjectRenderQueue(evt);
	mTrayMgr->frameRenderingQueued(evt);

	return true;
}

//void MainApplication::createRoot()
//{
//}
//
//void MainApplication::defineResources() const
//{
//}
//
//void MainApplication::setupRenderSystem() const
//{
//}
//
//void MainApplication::createWindow()
//{
//}
//
//void MainApplication::initResources()
//{
//}
//
//void MainApplication::createSceneMgr()
//{
//}

void MainApplication::createCamera()
{
	_camera = _sceneMgr->createCamera(k::CAMERA_NAME);
	_camera->setNearClipDistance(k::CAMERA_NEAR_DIST);

	auto *cameraNode = _sceneMgr->getRootSceneNode()->createChildSceneNode(k::CAMERA_NODE);
	cameraNode->attachObject(_camera);
	cameraNode->setPosition(k::CAMERA_START_POS);
	mCamController.SetStyle(CameraStyle::FREE_ROAM);
	mCamController.SetObject(cameraNode);
}

void MainApplication::createViewports() const
{
	auto *vp = _window->addViewport(_camera);
	vp->setBackgroundColour(k::VIEWPORT_BG_COLOUR);
	_camera->setAspectRatio(
		Ogre::Real(vp->getActualWidth()) /
		Ogre::Real(vp->getActualHeight())
	);
}

void MainApplication::createScene()
{
	_sceneMgr->setAmbientLight(k::AMBIENT_COLOUR);

	auto *light = _sceneMgr->createLight(k::MAIN_LIGHT);
	light->setPosition(k::MAIN_LIGHT_POS);

	// Setup ground, walls and ceiling
	Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);
	Ogre::Plane wall(Ogre::Vector3::UNIT_Z, 0);

	Ogre::MeshManager::getSingleton().createPlane(
		"plane",
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		plane,
		500, 500, 20, 20,
		true,
		1, 5, 5,
		Ogre::Vector3::UNIT_Z
	);
	Ogre::MeshManager::getSingleton().createPlane(
		"wall",
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		wall,
		500, 200, 20, 20,
		true,
		1, 5, 5,
		Ogre::Vector3::UNIT_Y
	);

	auto *groundEntity = _sceneMgr->createEntity("plane");
	_sceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(groundEntity);
	groundEntity->setMaterialName("Examples/Rockwall");

	auto *ceilingEntity = _sceneMgr->createEntity("plane");
	auto *ceilingNode = _sceneMgr->getRootSceneNode()->createChildSceneNode();
	ceilingNode->attachObject(ceilingEntity);
	ceilingEntity->setMaterialName("Examples/Checker");
	ceilingNode->rotate(Ogre::Vector3::UNIT_X, Ogre::Degree(180));
	ceilingNode->translate(Ogre::Vector3(0, 200, 0));

	// Place walls
	Ogre::Entity *walls[4];
	Ogre::SceneNode *wallNodes[4];
	walls[0] = _sceneMgr->createEntity("wall");
	wallNodes[0] = _sceneMgr->getRootSceneNode()->createChildSceneNode();
	wallNodes[0]->attachObject(walls[0]);
	walls[0]->setMaterialName("Examples/BumpyMetal");
	wallNodes[0]->translate(Ogre::Vector3(0, 100, -250));

	walls[1] = _sceneMgr->createEntity("wall");
	wallNodes[1] = _sceneMgr->getRootSceneNode()->createChildSceneNode();
	wallNodes[1]->attachObject(walls[1]);
	walls[1]->setMaterialName("Examples/BumpyMetal");
	wallNodes[1]->rotate(Ogre::Vector3::UNIT_Y, Ogre::Degree(270));
	wallNodes[1]->translate(Ogre::Vector3(250, 100, 0));

	walls[2] = _sceneMgr->createEntity("wall");
	wallNodes[2] = _sceneMgr->getRootSceneNode()->createChildSceneNode();
	wallNodes[2]->attachObject(walls[2]);
	walls[2]->setMaterialName("Examples/BumpyMetal");
	wallNodes[2]->rotate(Ogre::Vector3::UNIT_Y, Ogre::Degree(90));
	wallNodes[2]->translate(Ogre::Vector3(-250, 100, 0));

	walls[3] = _sceneMgr->createEntity("wall");
	wallNodes[3] = _sceneMgr->getRootSceneNode()->createChildSceneNode();
	wallNodes[3]->attachObject(walls[3]);
	walls[3]->setMaterialName("Examples/BumpyMetal");
	wallNodes[3]->rotate(Ogre::Vector3::UNIT_Y, Ogre::Degree(180));
	wallNodes[3]->translate(Ogre::Vector3(0, 100, 250));



	// Place ogre
	auto *ogreEntity = _sceneMgr->createEntity(k::OGRE_ENTITY);
	auto *ogreNode = _sceneMgr->getRootSceneNode()->createChildSceneNode();
	ogreNode->attachObject(ogreEntity);
	ogreNode->translate(Ogre::Vector3(0, 50, 0));

	// Place robot
	auto *robotEntity = _sceneMgr->createEntity(k::ROBOT_ENTITY);
	auto *robotNode = _sceneMgr->getRootSceneNode()->createChildSceneNode(k::ROBOT_NODE);
	robotNode->attachObject(robotEntity);
	robotNode->rotate(Ogre::Vector3::UNIT_Y, Ogre::Degree(90));
	auto cameraNode = robotNode->createChildSceneNode(k::ROBOT_CAMERA_NODE);
	//cameraNode->attachObject(_camera);
	cameraNode->rotate(Ogre::Vector3::UNIT_Z, Ogre::Degree(-30));
	cameraNode->rotate(Ogre::Vector3::UNIT_Y, Ogre::Degree(270));
	cameraNode->translate(Ogre::Vector3(0, 75, 200), Ogre::Node::TS_LOCAL);
	robotNode->translate(Ogre::Vector3(100, 0, 0));

	// Place some other models
	Ogre::Entity *entities[5];
	Ogre::SceneNode *sceneNodes[5];

	entities[0] = _sceneMgr->createEntity("Barrel.mesh");
	sceneNodes[0] = _sceneMgr->getRootSceneNode()->createChildSceneNode();
	sceneNodes[0]->attachObject(entities[0]);
	sceneNodes[0]->setPosition(Ogre::Vector3(100, 50, 100));
	sceneNodes[0]->scale(Ogre::Vector3(6));

	entities[1] = _sceneMgr->createEntity("fish.mesh");
	sceneNodes[1] = _sceneMgr->getRootSceneNode()->createChildSceneNode();
	sceneNodes[1]->attachObject(entities[1]);
	sceneNodes[1]->setPosition(Ogre::Vector3(-100, 50, -100));
	sceneNodes[1]->scale(Ogre::Vector3(6));

	entities[2] = _sceneMgr->createEntity("ninja.mesh");
	sceneNodes[2] = _sceneMgr->getRootSceneNode()->createChildSceneNode();
	sceneNodes[2]->attachObject(entities[2]);
	sceneNodes[2]->setPosition(Ogre::Vector3(-100, 0, 100));
	sceneNodes[2]->scale(Ogre::Vector3(0.5));

	entities[3] = _sceneMgr->createEntity("jaiqua.mesh");
	sceneNodes[3] = _sceneMgr->getRootSceneNode()->createChildSceneNode();
	sceneNodes[3]->attachObject(entities[3]);
	sceneNodes[3]->rotate(Ogre::Vector3::UNIT_Y, Ogre::Degree(180));
	sceneNodes[3]->setPosition(Ogre::Vector3(100, 50, -200));
	sceneNodes[3]->scale(Ogre::Vector3(3));

	entities[4] = _sceneMgr->createEntity("razor.mesh");
	sceneNodes[4] = _sceneMgr->getRootSceneNode()->createChildSceneNode();
	sceneNodes[4]->attachObject(entities[4]);
	sceneNodes[4]->setPosition(Ogre::Vector3(150, 50, -150));
	//sceneNodes[4]->scale(Ogre::Vector3(3));



	auto &overlayManager = Ogre::OverlayManager::getSingleton();
	// Create an overlay
	auto *overlay = overlayManager.create("OverlayName");

	// Create a panel
	auto *panel = static_cast<Ogre::OverlayContainer*>(overlayManager.createOverlayElement("Panel", "PanelName"));
	panel->setPosition(0.475, 0.4975);
	panel->setDimensions(0.05, 0.005);
	panel->setMaterialName("BaseWhite");

	auto *panel2 = static_cast<Ogre::OverlayContainer*>(overlayManager.createOverlayElement("Panel", "PanelName2"));
	panel2->setPosition(0.49725, 0.475);
	panel2->setDimensions(0.0045, 0.05);
	panel2->setMaterialName("BaseWhite");

	// Add the panel to the overlay
	overlay->add2D(panel);
	overlay->add2D(panel2);

	// Show the overlay
	overlay->show();
}

void MainApplication::setupOIS()
{
	Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
	OIS::ParamList pl;
	size_t windowHnd = 0;
	std::ostringstream windowHndStr;

	_window->getCustomAttribute("WINDOW", &windowHnd);
	windowHndStr << windowHnd;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

	_inputMgr = OIS::InputManager::createInputSystem(pl);

	_keyboard = static_cast<OIS::Keyboard*>(_inputMgr->createInputObject(OIS::OISKeyboard, true));
	_mouse = static_cast<OIS::Mouse*>(_inputMgr->createInputObject(OIS::OISMouse, true));
}

void MainApplication::setupListeners()
{
	windowResized(_window);

	Ogre::WindowEventUtilities::addWindowEventListener(_window, this);

	_root->addFrameListener(this);

	_mouse->setEventCallback(this);
	_keyboard->setEventCallback(this);
}
