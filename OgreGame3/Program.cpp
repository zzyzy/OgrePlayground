//
// OgreGame3/Program.cpp
// 
// Application entry point
//
// Author: Zhen Zhi Lee <leezhenzhi@gmail.com>
//
// Copyright (C) 2016 Zhen Zhi Lee.
//

#include "MainApplication.hpp"

int main()
{
	try
	{
		MainApplication app;
	}
	catch (Ogre::Exception ex)
	{
		std::cerr << ex.getFullDescription().c_str() << std::endl;
	}

	return 0;
}
