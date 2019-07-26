#pragma once
///////////////////////////////////////////////////////////////////////////
// Dependencies.h   : searches for dependencies in a file				 //
// ver 1.0                                                               //
//                                                                       //                      
// Author		 : Ashwini Prabhu, Syracuse University					 //
//					adprabhu@syr.edu									 //
///////////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
* =======================
* This package supports insertion of links to all the files in the directory tree
* on which the analysed file depends.
*
*  Required Files:
* =======================
*  DependencyTable.h
*  Logger.h 
*  FileSystem.h 
*
*  Maintainence History:
* =======================
*  ver 2.0 - 11 Mar 2019
*  - first release
*/
#include "../DependencyTable/DependencyTable.h"
#include "../Logger/Logger.h"
#include "../FileSystem/FileSystem.h"
#include <iostream>
#include <string>
#include <regex>
#include <fstream>
#include <conio.h>

using namespace Logging;
using namespace FileSystem;

class Dependencies
{
public:
	Dependencies();
	Dependencies(const DependencyTable& dt);

	void searchDependencies(std::string file, std::vector<std::string> filesList);
	void setDepTable(const DependencyTable& dt);
	void displayDependencies();
	DependencyTable getDependencies();

private:
	DependencyTable dt_;
	std::string str_ = "#include \"";
};

// -----< default ctor >--------------------------------------------------
Dependencies::Dependencies()
{
}

// -----< ctor to set dep table >-----------------------------------------
Dependencies::Dependencies(const DependencyTable& dt) : dt_(dt)
{
	Dbug::write("\n Initializing codeconverter with DependencyTable\n");
}

// -----< set dependency table function >---------------------------------
void Dependencies::setDepTable(const DependencyTable& dt)
{
	dt_ = dt;
}

// -----< get dependency table function >---------------------------------
DependencyTable Dependencies::getDependencies() {
	return dt_;
}

// -----< display dependency table >---------------------------------
void Dependencies::displayDependencies()
{
	dt_.display();
}

// -----< searches dependency for each file >---------------------------------
void Dependencies::searchDependencies(std::string file, std::vector<std::string> filesList)
{
	std::ifstream input(file);
	std::string line;
	bool foundDependency = false;
	
	while (std::getline(input, line)) {
		if (line.find(str_) != std::string::npos) {
			std::regex e("(#include)([^ ]*)");
			std::regex e2("(\")([^ ]*)");
			
			std::string dep = std::regex_replace(std::regex_replace(std::regex_replace(line, e2, "$2"), e2, "$2"), e, "$2");
			
			std::string filename = Path::getName(dep);
			for (auto foundFile : filesList) {
				if (filename.find(Path::getName(foundFile)) != filename.npos) {
					foundDependency = true;
					dt_.addDependency(file, dep);
				}
			}
		}
	}
	
	if(!foundDependency)
		dt_.addDependency(file, "");
}
