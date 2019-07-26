////////////////////////////////////////////////////////////////////////
// Executive.cpp													  //	
// Ver 3.0														      //
// Language:    C++, Visual Studio 2017                               //
// Source:      Jim Fawcett, Syracuse University					  //
//              jfawcett@twcny.rr.com					              //
// Author:      ASHWINI PRABHU, CSE687 - Object Oriented Design		  //
//				Spring 2019											  //
///////////////////////////////////////////////////////////////////////
/*
 *
 * Package Operations:
 * -------------------
 * Executive.cpp demonstrates all the functional requirements.
 *
 * Required packages:
 * -------------------
 * FileSystem.h					      // Directory and Path classes
 * CodeUtilities.h                   // ProcessCmdLine class
 * StringUtilities.h
 * Converter.h Dependencies.h DependencyTable.h
 * Display.h Parser.h Toker.h Semi.h
 * ActionsAndRules.h ConfigureParser.h
 *
 * Maintenance History:
 * --------------------
 * ver 3.0 : 11 April 2019
 * - third release
 * ver 2.0 : 11 Mar 2019
 * - second release
 * ver 1.0 : 06 Feb 2019
 * - first release
*/

#include "Executive.h"
#include <iostream>
#include <conio.h>
#include <filesystem>
#include <vector>
#include "../Converter/Converter.h"
#include "../DependencyTable/DependencyTable.h"
#include "../Utilities/CodeUtilities/CodeUtilities.h"
#include "../DirExplorer-Naive/DirExplorerN.h"
#include "../Dependencies/Dependencies.h"
#include "../Display/Display.h"
#include "../Tokenizer/Toker.h"
#include "../SemiExpression/Semi.h"
#include "Parser.h"
#include "ActionsAndRules.h"
#include "ConfigureParser.h"

using namespace Utilities;
using namespace CodeAnalysis;
#define Util StringHelper

// ---------< extract files >-------------------------
std::vector<std::string> Executive::extractFiles(std::vector<std::string> regexes, std::string paths, std::vector<std::string> patterns, bool hasOption)
{
	std::vector<std::string> files = loadObj_.findFiles(regexes, paths, patterns, hasOption, destinationPath);
	return files;
}

// ---------< search dependencies and convert files >-------------------------
void Executive::convertFiles(std::vector<std::string> filesList)
{
	DependencyTable dt;
	Dependencies d(dt);
	
	for (auto file : filesList)
	{
		std::cout << "\nFound file: " << file << "\n";
		d.searchDependencies(file, filesList);
	}
	
	// Find dependency table
	dt = d.getDependencies();
	
	// Set converter object table with the files found
	cc_.setDepTable(dt);
	
	// Start the converter
	cc_.convert();
	
	// List of converted files
	std::cout << "\nConverted files List \n";
	std::vector<std::string> convertedFilesList = cc_.convertedFiles();
	for (auto file : convertedFilesList)
	{
		std::cout << "\nConverted file: " << file << "\n";
	}
}

// ---------< display converted files >-------------------------
void Executive::displayFiles()
{
	Display disp;
	disp.display(cc_.convertedFiles());
}

// ---------< parse files >-------------------------
void CodeConverter::parseFiles(std::string file)
{
	classLineNumber_.clear();
	std::string fileSpec;
	fileSpec = FileSystem::Path::getFullFileSpec(file);
	std::string msg = "Processing file" + fileSpec;
	ConfigParseForCodeAnal configure;
	Parser* pParser = configure.Build();
	std::string name;
	try
	{
		if (pParser)
		{
			name = FileSystem::Path::getName(file);
			if (!configure.Attach(fileSpec))
			{
				std::cout << "\n  could not open file " << name << std::endl;
			}
		}
		else
		{
			std::cout << "\n\n  Parser not built\n\n";
		}
		Repository* pRepo = Repository::getInstance();
		pRepo->package() = name;
		while (pParser->next())
		{
			pParser->parse();
		}
		std::cout << "\n";
		ASTNode* pGlobalScope = pRepo->getGlobalScope();
		complexityEval(pGlobalScope);
		// Walk AST
		classLineNumber_ = TreeWalk(pGlobalScope, classLineNumber_, "class");
	}
	catch (std::exception& ex)
	{
		std::cout << "\n\n    " << ex.what() << "\n\n";
		std::cout << "\n  exception caught at line " << __LINE__ << " ";
		std::cout << "\n  in package \"" << name << "\"";
	}
	std::cout << "\n";
}

// -----< command line usage >----------------------------------------------
ProcessCmdLine::Usage customUsage()
{
	std::string usage;
	usage += "\n  Command Line: path [/option]* [/pattern]* [/regex]*";
	usage += "\n    path is relative or absolute path where processing begins";
	usage += "\n    [/option]* are one or more options of the form:";
	usage += "\n      /s     - walk directory recursively";
	usage += "\n      /demo  - run in demonstration mode (cannot coexist with /debug)";
	usage += "\n      /debug - run in debug mode (cannot coexist with /demo)";
	//usage += "\n      /h - hide empty directories";
	//usage += "\n      /a - on stopping, show all files in current directory";
	usage += "\n    [pattern]* are one or more pattern strings of the form:";
	usage += "\n      *.h *.cpp *.cs *.txt or *.*";
	usage += "\n    [regex] regular expression(s), i.e. [A-B](.*)";
	usage += "\n";
	return usage;
}

// -----< initialize loggers >------------------------
void initializeLoggers()
{
	Demo::attach(&std::cout);
	Demo::start();
}

// -----< stop loggers >-----------------------------------------------------
void stopLoggers()
{
	Demo::stop();
}

// -----< Factory for returning Executive as IExecutive* >--------------
IExecutive* objectFactory::createExecutive() {
	return new Executive;
}

// --------------- < Start class >-----------------------
void Executive:: Start()
{
	std::cout << "\n  Starting Executive Class \n";
}

// -------- < Sets Command Line arguments received from GUI >-----------------------
void Executive::setCmdArgs(std::string path, std::vector<std::string> filePattern, std::vector<std::string> regexPattern, bool hasOption) {
	std::cout << "\n  Setting CommandLine Arguments in Executive";
	paths = path;
	regexes = regexPattern;
	patterns = filePattern;
	hasSubDirOption = hasOption;
}

// -------- < Returns files that are converted to html >-----------------------
std::vector<std::string> Executive::getConvertedFileList() {
	std::cout << "\n  Converting files according to regex in Executive";
	std::vector<std::string> fileList = extractFiles(regexes, paths, patterns, hasSubDirOption);
	if (!fileList.empty()) {
		convertFiles(fileList);
	}
	return fileList;
}

// -----< loading files for conversion >---------------------------------------
std::vector<std::string> loadFiles(Executive& exeobj, std::vector<std::string> regexes, std::string paths, std::vector<std::string> patterns, bool hasOption)
{
	Demo::write("\n");
	Demo::write("\n Finding and loading all the files for conversion using Loader \n");
	std::vector<std::string> files_ = exeobj.extractFiles(regexes, paths, patterns, hasOption);
	return files_;
}

// -----< To convert and find dependencies >------------------------------
void convertFunction(Executive& exeObj, std::vector<std::string> filesList)
{
	Demo::write("\n");
	Demo::title("------------------ Convert and Dependency functionality------------------\n");
	exeObj.convertFiles(filesList);
}

// -----< For Display functionality >-------------------------------------
void displayFunction(Executive& exeObj)
{
	Demo::write("\n");
	Demo::title("------------------ Display functionality------------------\n");
	exeObj.displayFiles();
}


// -----< Test Stub for Executive >-------------------------------------
#ifdef TEST_Executive

// -----< DEMONSTRATION MAIN >---------------------------------------------
int main(int argc, char ** argv)
{
	initializeLoggers();
	Demo::title("Testing Executive functions");

	ProcessCmdLine pcl(argc, argv);
	pcl.usage(customUsage());

	if (pcl.parseError()) {
		pcl.usage();
		std::cout << "\n\n";
		return 1;
	}

	std::string paths = pcl.path();
	std::vector<std::string> regexes = pcl.regexes();
	std::vector<std::string> patterns = pcl.patterns();
	bool hasOption = pcl.hasOption('s');

	Executive exeObj;
	std::vector<std::string> filesList = loadFiles(exeObj, regexes, paths, patterns, hasOption);
	convertFunction(exeObj, filesList);
	displayFunction(exeObj);
	_getche();
	stopLoggers();
	return 0;
}

#endif