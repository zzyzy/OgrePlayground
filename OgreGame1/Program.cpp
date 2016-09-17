/*
*  OgreBasicTut/Program.cpp
*
*  Application entry point
*
*  Author: Zhen Zhi Lee <leezhenzhi@gmail.com>
*
*  Copyright (C) 2016 Zhen Zhi Lee.
*/

#include <iostream>
#include "Windows.h"
#include "MainApplication.hpp"

#ifdef __cplusplus
extern "C"
{
#endif
	int main(int argc, char* argv[])
	{
		MainApplication app;

		if (argc >= 2)
		{
			app.SetOgreHome(argv[1] != "" ? argv[1] : "");
		} 
		else
		{
			app.SetOgreHome(getenv("OGRE_HOME") != nullptr ? getenv("OGRE_HOME") : "");
		}

		try
		{
			app.Run();
		}
		catch (const Ogre::Exception &e)
		{
			MessageBox(nullptr, e.getFullDescription().c_str(), "An exception has occurred!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
			std::cerr << "An exception has occurred: " << e.getFullDescription().c_str() << std::endl;
		}

		return 0;
	}
#ifdef __cplusplus
}
#endif
