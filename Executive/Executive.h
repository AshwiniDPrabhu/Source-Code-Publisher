#pragma once
//////////////////////////////////////////////////////////////////////
// Executive.h														//
// ver 3.0															//
// Author:      ASHWINI PRABHU, CSE687 - Object Oriented Design		//
//				Spring 2019											//
//////////////////////////////////////////////////////////////////////
/*
 *
 * Package Operations:
 * -------------------
 * Executive.h uses the required packages and helps 
 * demonstrate all the functional requirements.
 *
 * Required packages:
 * -------------------
 * CodeUtilities.h                   
 * Loader.h
 * Converter.h 
 * Dependencies.h
 * Display.h 
 * IExecutive.h
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

#include "IExecutive.h"
#include "../Utilities/CodeUtilities/CodeUtilities.h"
#include "../DirExplorer-Naive/DirExplorerN.h"
#include "../Dependencies/Dependencies.h"
#include "../Loader/Loader.h"
#include "../Converter/Converter.h"
#include "../Display/Display.h"

class Executive : public IExecutive
{
public:
	std::vector<std::string> extractFiles(std::vector<std::string> regexes, std::string paths, std::vector<std::string> patterns, bool hasOption);
	void convertFiles(std::vector<std::string> files);
	void displayFiles();
	void Start() override;
	void setCmdArgs(std::string path, std::vector<std::string> filePattern, std::vector<std::string> regexPattern, bool hasOption) override;
	std::vector<std::string> getConvertedFileList() override;
private:
	Utilities::ProcessCmdLine *pcl_;
	Loader loadObj_;
	CodeConverter cc_;

	std::string dirIn_;
	std::string dirOut_;

	std::string destinationPath = "..\\ConvertedWebpages\\";

	std::string receivedMessages;

	std::string paths;
	std::vector<std::string> regexes;
	std::vector<std::string> patterns;
	bool hasSubDirOption;
};