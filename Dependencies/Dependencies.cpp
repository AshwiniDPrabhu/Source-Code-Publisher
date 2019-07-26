#pragma once
///////////////////////////////////////////////////////////////////////////
// Dependencies.cpp : provides test stub for Dependencies.h				 //
// ver 1.0                                                               //
//                                                                       // 
// Platform      : Visual Studio Community 2017, Windows 10 Pro x64		 //
// Author		 : Ashwini Prabhu, Syracuse University					 //
//					adprabhu@syr.edu									 //
///////////////////////////////////////////////////////////////////////////
#include "Dependencies.h"

using namespace Logging;

#ifdef TEST_DEPENDENCIES

int main()
{
	Dbug::attach(&std::cout);
	Dbug::start();

	Dbug::title("Testing dependencies functions");

	//Dbug::stop(); // do not log DependencyTable logs
	DependencyTable dt;

	Dependencies d(dt);
	
	std::string file = "..\\Logger\\Logger.h";
	d.searchDependencies(file);
	d.displayDependencies();
	
	_getche();
	Dbug::stop();
	return 0;
}

#endif // TEST_DEPENDENCIES


