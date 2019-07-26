/////////////////////////////////////////////////////////////////////
// Loader.h															//
// ver 1.0                                                         //
// ASHWINI PRABHU, CSE687 - Object Oriented Design, Spring 2019    //
/////////////////////////////////////////////////////////////////////
/*
 *
 * Package Operations:
 * -------------------
 * Loader.h demonstrates finding and loading all the files matching regex pattern
 * for conversion, starting at a path and regex pattern specified by the Executive.
 *
 * Required packages:
 * -------------------
 * FileSystem.h					      // Directory and Path classes
 * CodeUtilities.h                   // ProcessCmdLine class
 * StringUtilities.h
 *
 * Maintenance History:
 * --------------------
 * ver 1.0 : 11 March 2019
  * - first release
*/
#pragma once
#include <iostream>
#include <conio.h>
#include <filesystem>
#include <vector>
#include "../FileSystem/FileSystem.h"
#include "../Utilities/CodeUtilities/CodeUtilities.h"
#include "../Utilities/StringUtilities/StringUtilities.h"
#include "../DirExplorer-Naive/DirExplorerN.h"

using namespace FileSystem;
using namespace Utilities;

class Loader
	{
	public:
		std::vector<std::string> findFiles(std::vector<std::string> regexes, std::string paths, std::vector<std::string> patterns, bool hasOptions, std::string destinationPath);
	};

// --------------------< finds files matching regex pattern >-----------------------------------
	std::vector<std::string> Loader::findFiles(std::vector<std::string> regexes, std::string paths, std::vector<std::string> patterns, bool hasOptions, std::string destinationPath)
	{
		bool foundFlag = false;
		std::vector<std::string> filesToProcess;

		DirExplorerN de(paths);
		for (auto patt : patterns) {
			de.addPattern(patt);
		}
		if (hasOptions) {
			de.recurse();
		}
		de.search();
		de.showStats();

		std::cout << "\n\n" << std::endl;

		auto files = de.fileToProcess();
		auto filePaths = de.fileDirToProcess();

		for (auto regex_str : regexes) {
			std::regex r(regex_str);
			for (size_t i = 0; i < files.size(); ++i) {
				std::string filename = FileSystem::Path::getName(files[i]);
				std::string filePath = filePaths[i];
				if (std::regex_match(filename, r)) {
					foundFlag = true;
					std::cout << "\nFile matching regex pattern : " << filePath + "\\" + filename << std::endl;
					filesToProcess.push_back(filePath + "\\" + filename);
				}
			}
		}
		if (foundFlag == false) {
			std::cout << "No file matches the regex pattern!" << "\n";
		}
		return filesToProcess;
	}

