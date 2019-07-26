///////////////////////////////////////////////////////////////////////////
// Converter.cpp : implements and provides test stub for Converter.h     //
// ver 2.0                                                               //
//                                                                       // 
// Author		 : Ashwini Prabhu, Syracuse University					 //
//					adprabhu@syr.edu									 //
// Source        : Ammar Salman, Syracuse University                     //
//                 313/788-4694, assalman@syr.edu                        //
///////////////////////////////////////////////////////////////////////////

#include "Converter.h"
#include "../Logger/Logger.h"
#include "../FileSystem/FileSystem.h"
#include <conio.h>

using namespace FileSystem;
using namespace Logging;

// -----< default ctor >--------------------------------------------------
CodeConverter::CodeConverter()
{
}

// -----< ctor to set dep table >-----------------------------------------
CodeConverter::CodeConverter(const DependencyTable & dt) : dt_(dt)
{
	Dbug::write("\n  CodeConverter initialized with DependencyTable\n");
}

// -----< set dependency table function >---------------------------------
void CodeConverter::setDepTable(const DependencyTable & dt)
{
	dt_ = dt;
}

// -----< convert pre-set dependency table >------------------------------
std::vector<std::string> CodeConverter::convert()
{
	if (!createOutpurDir()) {
		Dbug::write("\n\n  Failed to start conversion due to inability to create output directory");
		Dbug::write("\n  -- Output Directory: " + Path::getFullFileSpec(outputDir_));
		return std::vector<std::string>();
	}

	Demo::write("\n\n  Converting files to webpages");
	Dbug::write("\n\n  Converting files to webpages");

	Dbug::write("\n  Output directory: " + Path::getFullFileSpec(outputDir_));
	Demo::write("\n  Output directory: " + Path::getFullFileSpec(outputDir_));

	for (auto entry : dt_) {
		if (convertFile(entry.first)) {
			Demo::write("\n  -- Converted: ");
			Dbug::write("\n  -- Converted: ");
		}
		else {
			Demo::write("\n  -- Failed:    ");
			Dbug::write("\n  -- Failed:    ");
		}
		std::string filename = Path::getName(entry.first);
		Dbug::write(filename);
		Demo::write(filename);
		Dbug::write("\n");
		Demo::write("\n");
	}
	Dbug::write("\n");
	Demo::write("\n");
	return convertedFiles_;
}

// -----< convert single file given path >----------------------------------
std::string CodeConverter::convert(const std::string & filepath)
{
	if (!createOutpurDir()) {
		Dbug::write("\n\n  Failed to start conversion due to inability to create output directory");
		Dbug::write("\n  -- Output Directory: " + Path::getFullFileSpec(outputDir_));
		return "";
	}

	Demo::write("\n\n  Converting files to webpages");
	Dbug::write("\n\n  Converting files to webpages");

	Dbug::write("\n  Output directory: " + Path::getFullFileSpec(outputDir_));
	Demo::write("\n  Output directory: " + Path::getFullFileSpec(outputDir_));

	if (convertFile(filepath)) {
		Demo::write("\n  -- Converted: ");
		Dbug::write("\n  -- Converted: ");
	}
	else {
		Demo::write("\n  -- Failed:    ");
		Dbug::write("\n  -- Failed:    ");
	}

	std::string filename = Path::getName(filepath);
	Dbug::write(filename);
	Demo::write(filename);

	return filename + ".html";
}

// -----< convert vector of files >----------------------------------
std::vector<std::string> CodeConverter::convert(const std::vector<std::string>& files)
{
	clear();
	dt_ = DependencyTable(files);
	return convert();
}

// -----< get set output directory >----------------------------------------
const std::string & CodeConverter::outputDir() const
{
	return outputDir_;
}

// -----< set output directory >--------------------------------------------
void CodeConverter::outputDir(const std::string & dir)
{
	outputDir_ = dir;
}

// -----< create output directory >-----------------------------------------
/* returns success of creation. it could fail if dir is sys protected */
bool CodeConverter::createOutpurDir()
{
	if (!Directory::exists(outputDir_)) {
		Dbug::write("\n  Creating output directory. Path: " + Path::getFullFileSpec(outputDir_));
		return Directory::create(outputDir_);
	}
	Dbug::write("\n  Output directory already exists. Proceeding...");
	return true;
}

// -----< get list of converted files >-------------------------------------
const std::vector<std::string> CodeConverter::convertedFiles() const
{
	return convertedFiles_;
}

void CodeConverter::clear()
{
	convertedFiles_.clear();
	dt_.clear();
}

// -----< private - read file and create webpage >--------------------------
bool CodeConverter::convertFile(std::string file)
{
	std::ifstream in(file);
	if (!in.is_open() || !in.good()) {
		Dbug::write("Error -- unable to read file, path may be invalid.");
		in.close();
		return false;
	}

	std::string filename = Path::getName(file);
	std::string outputPath = outputDir_ + filename + ".html";
	out_.open(outputPath, std::ofstream::out);
	if (!out_.is_open() || !out_.good()) {
		Dbug::write("Error -- unable to open output file for writing.");
		in.close();
		return false;
	}

	Dbug::stop();
	Demo::stop();
	parseFiles(file);

	Demo::start();
	Dbug::start();

	// write everything to file 
	addPreCodeHTML(filename);
	addDependencyLinks(file);
	addPreTag();

	int lineCount = 1;
	while (in.good()) {
		std::string line;
		while (std::getline(in, line)) {
			skipSpecialChars(line);
			handleComments(line);
			handleClassesFunctions(line, lineCount);
			out_ << line << std::endl;
			lineCount += 1;
		}
	}
	addClosingTags();
	out_.close();

	convertedFiles_.push_back(outputPath);
	return true;
}

// -----< wraps classes and functions with div blocks >--------------------------
void CodeConverter::handleClassesFunctions(std::string &line, int lineCount) {
	if (classLineNumber_.find(lineCount) != classLineNumber_.end()) {
		std::string functionLine = "<button class=\"function-button\" id=\"function-" + std::to_string(lineCount) + "-button\" onclick = toggleVisibilityByID('function-" + std::to_string(lineCount) + "')>-</button><div id=\"function-" + std::to_string(lineCount) + "\" class= \"function\">";
		std::string classLine = "<button class=\"class-button\" id=\"class-" + std::to_string(lineCount) + "-button\" onclick = toggleVisibilityByID('class-" + std::to_string(lineCount) + "')>-</button><div id=\"class-" + std::to_string(lineCount) + "\" class= \"class\">";
		std::string endLine = "</div>";

		if (classLineNumber_[lineCount] == "class") {
			size_t pos = line.find('{');
			line = line.insert(pos, classLine);
		}

		if (classLineNumber_[lineCount] == "end")
			line = line.append(endLine);

		if (classLineNumber_[lineCount] == "function") {
			size_t pos = line.find('{');
			line = line.insert(pos, functionLine);
		}

		if (classLineNumber_[lineCount] == "function-end") {
			size_t pos = line.find('{');
			line = line.insert(pos, functionLine);
			line = line.append(endLine);
		}

		if (classLineNumber_[lineCount] == "class-end") {
			size_t pos = line.find('{');
			line = line.insert(pos, classLine);
			line = line.append(endLine);
		}
	}
}

// -----< private - add generic HTML preliminary markup >-------------------
void CodeConverter::addPreCodeHTML(const std::string& title)
{
	out_ << "<DOCTYPE !HTML>" << std::endl;
	out_ << "<html>" << std::endl;
	out_ << "  <head>" << std::endl;
	out_ << "    <Title>" << title << "</Title>" << std::endl;
	out_ << "    <style>" << std::endl;
	out_ << "      body {" << std::endl;
	out_ << "        padding:15px 40px;" << std::endl;
	out_ << "        font-family: Consolas;" << std::endl;
	out_ << "        font-size: 1.25em;" << std::endl;
	out_ << "        font-weight: normal;" << std::endl;
	out_ << "      }" << std::endl;
	out_ << "      </style>" << std::endl;
	out_ << "      <script src=\"../script.js\"></script>" << std::endl;
	out_ << "  </head>" << std::endl << std::endl;
	out_ << "  <body>" << std::endl;
	out_ << "<label><input type=\"checkbox\" id=\"function-button\" value=\"hidefuncs\" checked=\"true\" onclick=\"toggleVisibilityFunction('function')\">Functions</label><br>" << std::endl;
	out_ << "<label><input type=\"checkbox\" id=\"class-button\" value=\"hideclass\" checked=\"true\" onclick=\"toggleVisibilityClass('class')\">Classes</label><br>" << std::endl;
	out_ << "<label><input type=\"checkbox\" id=\"comments-button\" value=\"hidecomms\" checked=\"true\" onclick =\"toggleVisibilityComments('comments')\">Comments</label><br>" << std::endl;
}

// -----< private - add pre tag >------------------------------------------
/* seperated into seperate function to allow for dependencies addition
*  before the actual code of the file */
void CodeConverter::addPreTag()
{
	out_ << "    <pre>" << std::endl;
}

// -----< private - add depedency links markup code >----------------------
void CodeConverter::addDependencyLinks(std::string file)
{
	std::string filename = Path::getName(file);
	if (!dt_.has(file)) { // in case of single file conversion
		Dbug::write("\n    No entry found in DependencyTable for [" + filename + "]. Skipping dependency links..");
		return;
	}

	if (dt_[file].size() == 0) { // in case the file has no dependencies
		Dbug::write("\n    No dependencies found for [" + filename + "]. Skipping dependency links..");
		return;
	}

	out_ << "    <h3>Dependencies: " << std::endl;
	for (auto dep : dt_[file]) {
		out_ << "      <a href=\"" << Path::getName(dep) << ".html\">" << Path::getName(dep) << "</a>" << std::endl;
	}
	out_ << "    </h3>";
}

// -----< private - add generic HTML markup closing tags >-----------------
void CodeConverter::addClosingTags()
{
	out_ << "    </pre>" << std::endl;
	out_ << "  </body>" << std::endl;
	out_ << "</html>" << std::endl;
}

// -----< private - replace HTML special chars >---------------------------
/* note: the function uses while loop to make sure ALL special characters
*  are replaced instead of just the first encounter. */
void CodeConverter::skipSpecialChars(std::string & line)
{
	size_t pos = line.npos;
	while ((pos = line.find('<')) != line.npos)
		line.replace(pos, 1, "&lt;");

	while ((pos = line.find('>')) != line.npos)
		line.replace(pos, 1, "&gt;");
}

// -----< wraps single line and multiline comments with div blocks >--------------------------
void CodeConverter::handleComments(std::string &line) {
	if (line.find("*/") != line.npos) {
		line += "</div>";
	}

	size_t pos = line.find('/');

	if (pos == line.npos) return;
	if (pos == line.size() - 1) return;
	if (line[pos + 1] == '/') {
		std::string newLine;
		newLine = line;
		newLine.insert(pos, "<div class= \"comments\">");
		newLine += "</div>";
		
		line = newLine;
	}

	if (line[pos + 1] == '*') {
		std::string newLine;
		newLine = line;
		newLine.insert(pos, "<div class= \"comments\">");
		line = newLine;
	}
}

#ifdef TEST_CONVERTER

// -----< test stub for converter class >-----------------------------------
int main() {
	Dbug::attach(&std::cout);
	Dbug::start();

	Dbug::title("Testing CodeConverter functions");

	Dbug::stop(); // do not log DependencyTable logs

	DependencyTable dt;
	dt.addDependency("Converter.h", "Converter.cpp");
	dt.addDependency("Converter.cpp", "Converter.h");
	dt.addDependency("A.h", "A.h");

	Dbug::start(); // log from now on

	CodeConverter cc(dt);
	dt.display();
	Dbug::write("\n");
	//cc.outputDir("C:\\windows\\system32\\"); // will trigger errors which are handled
	cc.convert();
	_getche();
	Dbug::stop();
	return 0;
}

#endif