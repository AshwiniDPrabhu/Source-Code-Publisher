#pragma once
///////////////////////////////////////////////////////////////////////////
// Converter.h   : defines source code conversion to webpage functions   //
// ver 2.0                                                               //
//                                                                       //                      
// Author		 : Ashwini Prabhu, Syracuse University					 //
//					adprabhu@syr.edu									 //
// Source        : Ammar Salman, Syracuse University                     //
//                 313/788-4694, assalman@syr.edu                        //
///////////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
* =======================
*  This package defines CodeConverter class which accepts DependencyTable
*  and uses its relationships to create linked webpages that point to each
*  other based on dependencies. It can also convert single files if needed.
*  The conversion process filters HTML special characters before printing
*  them into output files. The resulting output of this converter is a list
*  of files (vector<string>) of the created webpages.
*
*  Required Files:
* =======================
*  Converter.h Converter.cpp DependencyTable.h DependencyTable.cpp
*  Logger.h Logger.cpp FileSystem.h FileSystem.cpp
*
*  Maintainence History:
* =======================
*  ver 2.0 - 11 Mar 2019
*  - second release
*  - added handleComments() and handleClassesFunctions()
*  ver 1.0 - 11 Feb 2019
*  - first release
*/

#include "../DependencyTable/DependencyTable.h"
#include <vector>
#include <string>
#include <fstream>
#include <unordered_map>

class CodeConverter
{
public:
	CodeConverter();
	CodeConverter(const DependencyTable& dt);

	const std::string& outputDir() const;
	void outputDir(const std::string& dir);
	bool createOutpurDir();

	void setDepTable(const DependencyTable& dt);

	std::vector<std::string> convert();
	std::string convert(const std::string& filepath);
	std::vector<std::string> convert(const std::vector<std::string>& files);

	const std::vector<std::string> convertedFiles() const;

	void clear();

private:
	bool convertFile(std::string file);
	void addPreCodeHTML(const std::string& title);
	void parseFiles(std::string file);
	void addPreTag();
	void addDependencyLinks(std::string file);
	void addClosingTags();
	void skipSpecialChars(std::string& line);
	void handleComments(std::string &line);
	void handleClassesFunctions(std::string &line, int lineCount);

private:
	DependencyTable dt_;
	std::string outputDir_ = "..\\ConvertedWebpages\\";
	std::vector<std::string> convertedFiles_;
	std::ofstream out_;
	std::unordered_map<size_t, std::string> classLineNumber_;
};