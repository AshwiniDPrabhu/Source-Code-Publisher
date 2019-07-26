////////////////////////////////////////////////////////////////////////
// Loader.cpp - Test stub for Loader.h								  //	
// Ver 1.0														      //
// Language:    C++, Visual Studio 2017                               //
// Source:      Jim Fawcett, Syracuse University					  //
//              jfawcett@twcny.rr.com					              //
// Author:      ASHWINI PRABHU, CSE687 - Object Oriented Design		  //
//				Spring 2019											  //
///////////////////////////////////////////////////////////////////////
#pragma once
#include <iostream>
#include <conio.h>
#include <filesystem>
#include <vector>
#include "../FileSystem/FileSystem.h"
#include "../Utilities/CodeUtilities/CodeUtilities.h"
#include "../Utilities/StringUtilities/StringUtilities.h"
#include "../DirExplorer-Naive/DirExplorerN.h"
#include "Loader.h"

using namespace FileSystem;
using namespace Utilities;

#ifdef TEST_LOADER

ProcessCmdLine::Usage customUsage()
{
	std::string usage;
	usage += "\n  Command Line: path [/option]* [/pattern]*";
	usage += "\n    path is relative or absolute path where processing begins";
	usage += "\n    [/option]* are one or more options of the form:";
	usage += "\n      /s - walk directory recursively";
	//usage += "\n      /h - hide empty directories";
	//usage += "\n      /a - on stopping, show all files in current directory";
	usage += "\n    [pattern]* are one or more pattern strings of the form:";
	usage += "\n      *.h *.cpp *.cs *.txt or *.*";
	usage += "\n";
	return usage;
}

int main(int argc, char *argv[])
{
	Title("Demonstrating Loader Class Functionality");

	Loader loadObj;
	ProcessCmdLine pcl(argc, argv);
	pcl.usage(customUsage());

	preface("Command Line: ");
	pcl.showCmdLine();
	putline();

	if (pcl.parseError())
	{
		pcl.usage();
		std::cout << "\n\n";
		return 1;
	}

	std::string destinationPath = "..\\convertedPages\\";
	std::string paths = pcl.path();
	std::vector<std::string> regexes = pcl.regexes();
	std::vector<std::string> patterns = pcl.patterns();
	bool hasOption = pcl.hasOption('s');

	/*----<  Call the find function to obtain and convert the files >-----*/
	std::vector<std::string> filesFound = loadObj.findFiles(regexes, paths, patterns, hasOption, destinationPath);

	_getche();
	std::cout << "\n\n";
	std::cout << "\n\n";
	return 0;
}

#endif
