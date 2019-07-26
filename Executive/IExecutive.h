#pragma once
//////////////////////////////////////////////////////////////////////
// IExecutive.h														//
// ver 1.0															//
// Author:      ASHWINI PRABHU, CSE687 - Object Oriented Design		//
//				Spring 2019											//
//////////////////////////////////////////////////////////////////////
/*
 *
 * Package Operations:
 * -------------------
 * IExecutive provides an interface for Executive class.
 * It consists of an object factory which instantiates the executive class.
 *
 * Maintenance History:
 * --------------------
 * ver 1.0 : 11 April 2019
 * - first release
*/

#include <vector>
#include <string>

class IExecutive{
	public:
		virtual void Start() = 0;
		virtual void setCmdArgs(std::string path, std::vector<std::string> filePattern, std::vector<std::string> regexPattern, bool hasOption) = 0;
		virtual std::vector<std::string> getConvertedFileList() = 0;
		virtual ~IExecutive() {}
};

extern "C" {
	struct objectFactory {
		IExecutive* createExecutive();
	};
}

